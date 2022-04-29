#include <task.h>
#include <lock.h>
#include "../drivers/serial.h"
#include "../cpu/gdt.h"
#include "../timer.h"
#include <sys.h>
#include <interrupts.h>
#include <io.h>
#include <debug.h>
#include <asm.h>

// Comment or uncomment this line to enable/disable the debug messages
// #define DEBUG_SCHEDULER 1

Vector<ProcessControlBlock *> ListProcess;

namespace MultiTasking
{
    MultiTasking *MultiProcessing = nullptr;

#define ScheduleInterrupt asm volatile("int $0x2b")
    NEWLOCK(scheduler_lock);
    NEWLOCK(exit_lock);
    NEWLOCK(process_lock);
    NEWLOCK(thread_lock);
    static uint64_t upcoming_pid = 0;
    static uint64_t upcoming_tid = 0;
    static uint64_t LastCount = 0;
    static bool IdleTrigger = false;
    static bool AllocProcEnable = true;
    static bool ScheduleOn = false;

    ProcessControlBlock *MultiTasking::GetCurrentProcess() { return CurrentProcess; }

    ThreadControlBlock *MultiTasking::GetCurrentThread() { return CurrentThread; }

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
        debug("Thread %d terminated", thread->ThreadID);
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
            debug("Process %d terminated", process->ProcessID);
            KernelPageTableAllocator->RemovePageTable(process->PageTable);
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

    extern "C"
    {
        void do_exit(uint64_t code)
        {
            LOCK(exit_lock);
#ifdef DEBUG_SCHEDULER
            debug("parent:%s tid:%d, code:%016p", CurrentProcess->Name, CurrentThread->ThreadID, code);
#endif
            CurrentThread->ExitCode = code;
            trace("Exiting thread %d...", CurrentThread->ThreadID);
            UNLOCK(exit_lock);
            CurrentThread->State = STATE_TERMINATED; // it may get stuck in lock if multiple threads are exiting at the same time.
            CPU_STOP;
        }
    }

    ProcessControlBlock *create_process(ProcessControlBlock *parent, char *name)
    {
        LOCK(process_lock);
#ifdef DEBUG_SCHEDULER
        debug("name: %s", name);
#endif
        ProcessControlBlock *process = new ProcessControlBlock;

        process->ProcessID = upcoming_pid++;
        process->Time = new ControlBlockTime;
        process->State = STATE_READY;
        process->Checksum = PROCESS_CHECKSUM;
        memcpy(process->Name, name, sizeof(process->Name));
        if (parent)
            process->Parent = parent;
        process->PageTable = KernelPageTableAllocator->CreatePageTable();
#ifdef DEBUG_SCHEDULER
        debug("%s address space: %#llx", name, process->PageTable);
#endif
        SetControlBlockTime(process->Time);
        if (parent != nullptr)
            parent->Children.push_back(process);
        ListProcess.push_back(process);
#ifdef DEBUG_SCHEDULER
        debug("New thread created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", process->ProcessID, process->Time->h, process->Time->m, process->Time->s, process->Time->d, process->Time->M, process->Time->y);
#endif
        UNLOCK(process_lock);
        return process;
    }

    ThreadControlBlock *create_thread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy, bool UserMode)
    {
        LOCK(thread_lock);
        if (parent->Checksum != PROCESS_CHECKSUM)
        {
            err("Thread cannot have a null parent!");
            return nullptr;
        }
#ifdef DEBUG_SCHEDULER
        debug("fct: %016p, Priority: %d, State: %d, Policy %d | child of \"%s\"", function, Priority, State, Policy, parent->Name);
#endif
        ThreadControlBlock *thread = new ThreadControlBlock;

        thread->Time = new ControlBlockTime;
        thread->ThreadID = upcoming_tid++;
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
#ifdef DEBUG_SCHEDULER
        debug("New thread created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", thread->ThreadID, thread->Time->h, thread->Time->m, thread->Time->s, thread->Time->d, thread->Time->M, thread->Time->y);
#endif
        UNLOCK(thread_lock);
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

    void SetPageTable(VMM::PageTable *PML)
    {
#ifdef DEBUG_SCHEDULER
        debug("Setting the new page table %#llx", PML);
#endif
        if ((uint64_t)PML != (uint64_t)readcr3().raw)
        {
            // TODO: check if the page table is valid
            // if (PML->Entries->GetAddress() == 0)
            // {
            //     err("Address space %#llx is not valid!", PML);
            //     return;
            // }
            asm volatile("mov %[PML], %%cr3"
                         :
                         : [PML] "q"(PML)
                         : "memory");
#ifdef DEBUG_SCHEDULER
            debug("Page table success!");
#endif
        }
#ifdef DEBUG_SCHEDULER
        else
        {
            debug("Page table failed! Is the same");
        }
#endif
    }

    __attribute__((noreturn)) static void IdleProcessLoop()
    {
#ifdef DEBUG_SCHEDULER
        debug("Idle process started!");
#endif
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

        InterruptHandler(multi_scheduler_interrupt_handler)
        {
            uint64_t timeslice = PRIORITY_MEDIUM; // cannot jump from this goto statement to its label on "goto scheduler_eoi;"
            if (!ScheduleOn)
                goto scheduler_eoi;

            LOCK(scheduler_lock);
            if (INT_NUM != IRQ11)
                panic("Something is wrong with Scheduler (corrupted registers)");

            if (!CurrentProcess || !CurrentThread)
            {
#ifdef DEBUG_SCHEDULER
                debug("Searching for new process/thread (P:%d T:%d)", CurrentProcess ? 1 : 0, CurrentThread ? 1 : 0);
#endif
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
#ifdef DEBUG_SCHEDULER
                        debug("Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
#endif
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

                if (CurrentThread->State == STATE_RUNNING)
                    CurrentThread->State = STATE_READY;

                uint64_t i;
                for (i = 0; i < CurrentProcess->Threads.size(); i++)
                {
                    check_thread(CurrentProcess->Threads[i]);
                    ThreadControlBlock *thread = CurrentProcess->Threads[(i + 1)];
                    check_thread(thread);
                    if (CurrentProcess->State != STATE_READY)
                        break;
                    if (thread->State != STATE_READY)
                        continue;
                    CurrentThread = thread;
                    timeslice = CurrentThread->Priority;
#ifdef DEBUG_SCHEDULER
                    debug("Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
#endif
                    goto scheduler_success;
                }

                for (uint64_t i = 0; i < ListProcess.size(); i++)
                {
                    check_process(ListProcess[i]);
                    ProcessControlBlock *process = ListProcess[(i + 1)];
                    check_process(ListProcess[(i + 1)]);
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
#ifdef DEBUG_SCHEDULER
                        debug("Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
#endif
                        goto scheduler_success;
                    }
                }

                for (uint64_t i = 0; i < ListProcess.size(); i++)
                    if (ListProcess[i] == CurrentProcess)
                        for (uint64_t p = 0; p < i + 1; p++)
                        {
                            check_process(ListProcess[p]);
                            ProcessControlBlock *process = ListProcess[p];
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
#ifdef DEBUG_SCHEDULER
                                debug("Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
#endif
                                goto scheduler_success;
                            }
                        }
            }

        scheduler_idle:
#ifdef DEBUG_SCHEDULER
            debug("Idling...");
#endif
            if (CurrentProcess != nullptr)
                remove_process(CurrentProcess);
            IdleProcess = create_process(nullptr, (char *)"idle");
            IdleThread = create_thread(IdleProcess, reinterpret_cast<uint64_t>(IdleProcessLoop), 0, 0, PRIORITY_VERYLOW, STATE_READY, POLICY_KERNEL, false);
            timeslice = IdleThread->Priority;
            CurrentProcess = IdleProcess;
            CurrentThread = IdleThread;
            *regs = IdleThread->Registers;
            // SetPageTable(IdleProcess->PageTable);
            Yield(timeslice);
            goto scheduler_end;
        scheduler_success:
#ifdef DEBUG_SCHEDULER
            debug("Switching...");
#endif
            CurrentThread->State = STATE_RUNNING;
            UpdateProcessTimeUsed(CurrentProcess->Time);
            UpdateProcessTimeUsed(CurrentThread->Time);
            *regs = CurrentThread->Registers;
            // SetPageTable(CurrentProcess->PageTable);
            wrmsr(MSR_FS_BASE, CurrentThread->Segment.fs);
            wrmsr(MSR_GS_BASE, (uint64_t)CurrentThread);
            wrmsr(MSR_SHADOW_GS_BASE, CurrentThread->UserMode ? CurrentThread->Segment.gs : (uint64_t)CurrentThread);
            Yield(timeslice);
        scheduler_end:
            UNLOCK(scheduler_lock);
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
