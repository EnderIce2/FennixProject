#include <internal_task.h>

#include "../drivers/serial.h"
#include "../cpu/gdt.h"
#include "../timer.h"
#include "fxsr.h"

#include <interrupts.h>
#include <critical.hpp>
#include <debug.h>
#include <sys.h>
#include <int.h>
#include <asm.h>
#include <io.h>

// Comment or uncomment this line to enable/disable the debug messages
// #define DEBUG_SCHEDULER 1

#ifdef DEBUG_SCHEDULER
#define schedbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define schedbg(m, ...)
#endif

Vector<ProcessControlBlock *> ListProcess;

namespace MultiTasking
{
    MultiTasking *MultiProcessing = nullptr;

#define ScheduleInterrupt asm volatile("int $0x2b")
    static uint64_t NextPID = 0;
    static uint64_t NextTID = 0;
    static uint64_t LastCount = 0;
    static bool IdleTrigger = false;
    static bool AllocProcEnable = true;
    static bool ScheduleOn = false;

    ProcessControlBlock *MultiTasking::GetCurrentProcess() { return CurrentProcess; }

    ThreadControlBlock *MultiTasking::GetCurrentThread() { return CurrentThread; }

    Vector<ProcessControlBlock *> MultiTasking::GetVectorProcessList() { return ListProcess; }

    void SetControlBlockTime(ControlBlockTime *Time)
    {
        Time->ticks_used = 0;
        Time->tick = counter();
        uint32_t timetmp = 0;
        outb(0x70, 0x00);
        timetmp = inb(0x71);
        Time->s = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outb(0x70, 0x02);
        timetmp = inb(0x71);
        Time->m = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outb(0x70, 0x04);
        timetmp = inb(0x71);
        Time->h = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outb(0x70, 0x07);
        timetmp = inb(0x71);
        Time->d = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outb(0x70, 0x08);
        timetmp = inb(0x71);
        Time->M = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outb(0x70, 0x09);
        timetmp = inb(0x71);
        Time->y = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
    }

    void remove_thread(ThreadControlBlock *thread)
    {
        for (uint64_t i = 0; i < thread->Parent->Threads.size(); i++)
            if (thread->Parent->Threads[i] == thread)
            {
                thread->Parent->Threads.remove(i);
                break;
            }
        schedbg("Thread %d terminated", thread->ThreadID);
        KernelStackAllocator->FreeStack(thread->Stack);
        kfree(thread);
        thread = nullptr;
    }

    void remove_process(ProcessControlBlock *process)
    {
        // TODO: add removal of child processes & and from parent child list
        if (process == nullptr)
            return;

        if (process->State == STATE_TERMINATED)
        {
            foreach (ThreadControlBlock *thread in process->Threads)
            {
                check_thread(thread);
                remove_thread(thread);
            }

            for (size_t i = 0; i < ListProcess.size(); i++)
            {
                if (ListProcess[i] == process)
                {
                    ListProcess.remove(i);
                    break;
                }
            }
            schedbg("Process %d terminated", process->ProcessID);
            KernelPageTableAllocator->RemovePageTable(reinterpret_cast<VMM::PageTable *>(process->PageTable));
            kfree(process);
            process = nullptr;
        }
        else
        {
            foreach (ThreadControlBlock *thread in process->Threads)
            {
                check_thread(thread);
                if (thread->State == STATE_TERMINATED)
                    remove_thread(thread);
            }

            // Should I remove the process if it's empty with no threads/processes?
        }
    }

    extern "C" void do_exit(uint64_t code)
    {
        EnterCriticalSection;
        CurrentThread->State = STATE_TERMINATED;
        CurrentThread->ExitCode = code;
        schedbg("parent:%s tid:%d, code:%016p", CurrentProcess->Name, CurrentThread->ThreadID, code);
        trace("Exiting thread %d...", CurrentThread->ThreadID);
        LeaveCriticalSection;
        CPU_STOP;
    }

    ProcessControlBlock *create_process(ProcessControlBlock *parent, char *name)
    {
        EnterCriticalSection;
        schedbg("name: %s", name);
        ProcessControlBlock *process = new ProcessControlBlock;

        process->ProcessID = NextPID++;
        process->Time = new ControlBlockTime;
        process->State = STATE_READY;
        process->Checksum = PROCESS_CHECKSUM;
        memcpy(process->Name, name, sizeof(process->Name));
        if (parent)
            process->Parent = parent;
        process->PageTable = KernelPageTableAllocator->CreatePageTable();
        schedbg("%s address space: %#llx", name, process->PageTable);
        SetControlBlockTime(process->Time);
        if (parent != nullptr)
            parent->Children.push_back(process);
        ListProcess.push_back(process);
        schedbg("New process created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", process->ProcessID, process->Time->h, process->Time->m, process->Time->s, process->Time->d, process->Time->M, process->Time->y);
        trace("New process %s created.", name);
        LeaveCriticalSection;
        return process;
    }

    ThreadControlBlock *create_thread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy, bool UserMode)
    {
        EnterCriticalSection;
        if (parent->Checksum != PROCESS_CHECKSUM)
        {
            err("Thread cannot have a null parent!");
            return nullptr;
        }
        schedbg("fct: %016p, Priority: %d, State: %d, Policy %d | child of \"%s\"", function, Priority, State, Policy, parent->Name);
        ThreadControlBlock *thread = new ThreadControlBlock;

        thread->Time = new ControlBlockTime;
        thread->Msg = new MessageQueue;
        thread->ThreadID = NextTID++;
        thread->Parent = parent;
        thread->State = State;
        thread->Policy = Policy;
        thread->Priority = Priority;
        thread->Checksum = THREAD_CHECKSUM;
        thread->UserMode = UserMode;

        memcpy(&thread->Registers, 0, sizeof(REGISTERS));
        if (!UserMode)
        {
            // Kernel Mode
            thread->Registers.ds = GDT_KERNEL_DATA;
            thread->Registers.cs = GDT_KERNEL_CODE;
            thread->Registers.ss = GDT_KERNEL_DATA;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
            thread->Stack = KernelStackAllocator->AllocateStack();
            thread->Registers.STACK = (uint64_t)thread->Stack;
            POKE(uint64_t, thread->Registers.rsp) = (uint64_t)do_exit;
        }
        else
        {
            // User Mode
            thread->Registers.ds = GDT_USER_DATA;
            thread->Registers.cs = GDT_USER_CODE;
            thread->Registers.ss = GDT_USER_DATA;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
            thread->Stack = KernelStackAllocator->AllocateStack(true);
            thread->Registers.STACK = (uint64_t)thread->Stack;
            // we can't just call the do_exit here. let the crt make a syscall for exiting
        }

        thread->Registers.FUNCTION = (uint64_t)function;
        thread->Registers.ARG0 = (uint64_t)args0; // args0
        thread->Registers.ARG1 = (uint64_t)args1; // args1

        thread->Segment.gs = UserMode ? 0 : (uint64_t)thread;
        thread->Segment.fs = 0;

        SetControlBlockTime(thread->Time);
        parent->Threads.push_back(thread);
        schedbg("New thread created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", thread->ThreadID, thread->Time->h, thread->Time->m, thread->Time->s, thread->Time->d, thread->Time->M, thread->Time->y);
        trace("New thread %d created (%s).", thread->ThreadID, parent->Name);
        LeaveCriticalSection;
        return thread;
    }

    ProcessControlBlock *MultiTasking::CreateProcess(ProcessControlBlock *parent, char *name)
    {
        return create_process(parent, name);
    }

    ThreadControlBlock *MultiTasking::CreateThread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy, bool UserMode)
    {
        return create_thread(parent, function, args0, args1, Priority, State, Policy, UserMode);
    }

    void SetPageTable(void *PML)
    {
        if ((uint64_t)PML != (uint64_t)readcr3().raw)
        {
            // schedbg("Setting the new page table %#llx", PML);
            // TODO: Page switching issue is only occurred when is running under QEMU. VMware and VirtualBox are not affected by this weird bug.
            // asm volatile("mov %[PML], %%cr3"
            //              :
            //              : [PML] "q"(PML)
            //              : "memory");
        }
    }

    __attribute__((noreturn)) static void IdleProcessLoop()
    {
        schedbg("Idle process started!");
    idle_proc_loop:
        // if (mwait_available)
        // mwait(0, 0);
        // else
        HLT;
        goto idle_proc_loop;
    }

    void UpdateProcessTimeUsed(ControlBlockTime *Time)
    {
        uint64_t CurrentCount = counter();
        uint64_t elapsed = LastCount - CurrentCount;
        LastCount = CurrentCount;
        Time->ticks_used += elapsed;
    }

    void MultiTasking::Schedule()
    {
        if (ScheduleOn)
            ScheduleInterrupt;
    }

    void Yield(uint64_t Timeslice)
    {
        // lapic_oneshot(SCHEDULER_INTERRUPT, Timeslice);
        uint32_t TimerClock = get_timer_clock();
        set_yield_schedule(counter() + ((Timeslice * 10000) * 1000000000) / TimerClock);
    }

    extern "C"
    {
        __attribute__((naked, used)) void multi_schedule_handler_helper()
        {
            asm("cld\n"
                "pushq %rax\n"
                "pushq %rbx\n"
                "pushq %rcx\n"
                "pushq %rdx\n"
                "pushq %rsi\n"
                "pushq %rdi\n"
                "pushq %rbp\n"
                "pushq %r8\n"
                "pushq %r9\n"
                "pushq %r10\n"
                "pushq %r11\n"
                "pushq %r12\n"
                "pushq %r13\n"
                "pushq %r14\n"
                "pushq %r15\n"
                "movq %ds, %rax\n"
                "pushq %rax\n"
                "movw $16, %ax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "movw %ax, %ss\n"
                "movq %rsp, %rdi\n"
                "call multi_scheduler_interrupt_handler\n"
                "popq %rax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "popq %r15\n"
                "popq %r14\n"
                "popq %r13\n"
                "popq %r12\n"
                "popq %r11\n"
                "popq %r10\n"
                "popq %r9\n"
                "popq %r8\n"
                "popq %rbp\n"
                "popq %rdi\n"
                "popq %rsi\n"
                "popq %rdx\n"
                "popq %rcx\n"
                "popq %rbx\n"
                "popq %rax\n"
                "addq $16, %rsp\n"
                "iretq");
        }

        // void fxsave(char *region)
        // {
        //     asm volatile("fxsave %0"
        //                  :
        //                  : "m"(region));
        // }

        // void fxrstor(char *region)
        // {
        //     asm volatile("fxrstor %0"
        //                  :
        //                  : "m"(region));
        // }

        InterruptHandler(multi_scheduler_interrupt_handler)
        {
            uint64_t timeslice = PRIORITY_MEDIUM; // cannot jump from this goto statement to its label on "goto scheduler_eoi;"
            Critical::CriticalSectionData *CriticalSectionData;
            if (!ScheduleOn)
                goto scheduler_eoi;

            CriticalSectionData = Critical::Enter();
            if (INT_NUM != IRQ11)
                panic("Something is wrong with Scheduler (corrupted registers)");

            if (!CurrentProcess || !CurrentThread)
            {
                schedbg("Searching for new process/thread (P:%d T:%d)", CurrentProcess ? 1 : 0, CurrentThread ? 1 : 0);
                foreach (ProcessControlBlock *process in ListProcess)
                {
                    check_process(process);
                    if (process->State != STATE_READY)
                    {
                        remove_process(process);
                        continue;
                    }

                    foreach (ThreadControlBlock *thread in process->Threads)
                    {
                        check_thread(thread);
                        if (thread->State != STATE_READY)
                            continue;
                        CurrentProcess = process;
                        CurrentThread = thread;
                        timeslice = thread->Priority;
                        schedbg("Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
                        goto scheduler_success;
                    }
                }
                goto scheduler_idle;
            }
            else
            {
                CurrentThread->Registers = *regs;
                CurrentThread->Segment.gs = rdmsr(MSR_SHADOW_GS_BASE);
                CurrentThread->Segment.fs = rdmsr(MSR_FS_BASE);
                fxsave(CurrentThread->fx_region);

                if (CurrentThread->State == STATE_RUNNING)
                    CurrentThread->State = STATE_READY;

                for (uint64_t i = 0; i < CurrentProcess->Threads.size(); i++)
                {
                    if (CurrentProcess->Threads[i] != CurrentThread)
                        continue;
                    ThreadControlBlock *thread = CurrentProcess->Threads[(i + 1)];
                    check_thread(thread);
                    if (CurrentProcess->State != STATE_READY)
                        break;
                    if (thread->State != STATE_READY)
                        continue;
                    CurrentThread = thread;
                    timeslice = CurrentThread->Priority;
                    schedbg("[thd 0 -> end] Scheduling thread %d parent of %s->%d Procs %d", thread->ThreadID, thread->Parent->Name, CurrentProcess->Threads.size(), ListProcess.size());
                    goto scheduler_success;
                }

                for (uint64_t i = 0; i < ListProcess.size(); i++)
                    if (ListProcess[i] == CurrentProcess)
                    {
                        check_process(ListProcess[i + 1]);
                        ProcessControlBlock *process = ListProcess[i + 1];
                        if (process->Threads.size() == 0)
                            continue;
                        if (process->State != STATE_READY)
                            continue;
                        for (uint64_t j = 0; j < process->Threads.size(); i++)
                        {
                            ThreadControlBlock *thread = process->Threads[j];
                            check_thread(thread);
                            if (thread->State != STATE_READY)
                                continue;
                            remove_process(CurrentProcess);
                            CurrentProcess = process;
                            CurrentThread = thread;
                            timeslice = CurrentThread->Priority;
                            schedbg("[cur proc+1 -> first thd] Scheduling thread %d parent of %s->%d (Total Procs %d)", thread->ThreadID, thread->Parent->Name, process->Threads.size(), ListProcess.size());
                            goto scheduler_success;
                        }
                    }

                for (uint64_t i = 0; i < ListProcess.size(); i++)
                {
                    ProcessControlBlock *process = ListProcess[i];
                    check_process(process);
                    if (process->Threads.size() == 0)
                        continue;
                    if (process->State != STATE_READY)
                        continue;

                    foreach (ThreadControlBlock *thread in process->Threads)
                    {
                        check_thread(thread);
                        if (thread->State != STATE_READY)
                            continue;
                        remove_process(CurrentProcess);
                        CurrentProcess = process;
                        CurrentThread = thread;
                        timeslice = CurrentThread->Priority;
                        schedbg("[proc 0 -> end -> first thd] Scheduling thread %d parent of %s->%d (Procs %d)", thread->ThreadID, thread->Parent->Name, process->Threads.size(), ListProcess.size());
                        goto scheduler_success;
                    }
                }
            }

        scheduler_idle:
            schedbg("Idling...");
            if (CurrentProcess != nullptr)
                remove_process(CurrentProcess);
            IdleProcess = create_process(nullptr, (char *)"idle");
            IdleThread = create_thread(IdleProcess, reinterpret_cast<uint64_t>(IdleProcessLoop), 0, 0, PRIORITY_VERYLOW, STATE_READY, POLICY_KERNEL, false);
            timeslice = IdleThread->Priority;
            CurrentProcess = IdleProcess;
            CurrentThread = IdleThread;
            *regs = IdleThread->Registers;
            SetPageTable(IdleProcess->PageTable);
            Yield(timeslice);
            goto scheduler_end;
        scheduler_success:
            CurrentThread->State = STATE_RUNNING;
            UpdateProcessTimeUsed(CurrentProcess->Time);
            UpdateProcessTimeUsed(CurrentThread->Time);
            *regs = CurrentThread->Registers;
            SetPageTable(CurrentProcess->PageTable);
            wrmsr(MSR_FS_BASE, CurrentThread->Segment.fs);
            wrmsr(MSR_GS_BASE, (uint64_t)CurrentThread);
            wrmsr(MSR_SHADOW_GS_BASE, CurrentThread->UserMode ? CurrentThread->Segment.gs : (uint64_t)CurrentThread);
            fxrstor(CurrentThread->fx_region);
            Yield(timeslice);
        scheduler_end:
            LeaveCriticalSection;
        scheduler_eoi:
            EndOfInterrupt(INT_NUM);
        }
    }

    void MultiTasking::ToggleScheduler(bool toggle) { ScheduleOn = toggle; }

    MultiTasking::MultiTasking()
    {
        // if (mwait_available)
        // {
        //     IdleTrigger = true;
        //     // monitor((uint64_t)&IdleTrigger, 0, 0);
        // }
        CurrentTaskingMode = TaskingMode::Multi;
    }

    MultiTasking::~MultiTasking()
    {
        // TODO: remove everything but idk what to say about this...
        ScheduleOn = false;
        CurrentTaskingMode = TaskingMode::None;
    }
};

void schedule() { ScheduleInterrupt; }
