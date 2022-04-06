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

namespace AdvancedTasking
{
    MultiTasking MultiProcessing;

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

    ProcessControlBlock *MultiTasking::GetCurrentProcess()
    {
        return CurrentProcess;
    }

    ThreadControlBlock *MultiTasking::GetCurrentThread()
    {
        return CurrentThread;
    }

    void AllocateProcess(ProcessControlBlock *Process)
    {
        if (AllocProcEnable)
            for (size_t i = 0; i < MAX_PROCESSES; i++)
            {
                if (ListProcess[i] == nullptr)
                {
                    ListProcess[i] = Process;
                    return;
                }
            }
        else
            return;
    }

    void FreeProcess(ProcessControlBlock *Process)
    {
        for (size_t i = 0; i < MAX_PROCESSES; i++)
            if (ListProcess[i] != nullptr)
                if (ListProcess[i] == Process)
                    ListProcess[i] = nullptr;
    }

    void set_cb_time(ControlBlockTime *Time)
    {
        Time->ticks_used = 0;
        Time->tick = counter();
        uint32_t timetmp = 0;
        outportb(0x70, 0x00);
        timetmp = inportb(0x71);
        Time->s = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outportb(0x70, 0x02);
        timetmp = inportb(0x71);
        Time->m = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outportb(0x70, 0x04);
        timetmp = inportb(0x71);
        Time->h = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outportb(0x70, 0x07);
        timetmp = inportb(0x71);
        Time->d = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outportb(0x70, 0x08);
        timetmp = inportb(0x71);
        Time->M = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
        outportb(0x70, 0x09);
        timetmp = inportb(0x71);
        Time->y = ((timetmp & 0x0F) + ((timetmp >> 4) * 10));
    }

    void alloc_thread_in_proc(ThreadControlBlock *array[], ThreadControlBlock *item)
    {
        for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            if (array[i] == nullptr)
            {
                array[i] = item;
                return;
            }
    }

    void free_thread_in_proc(ThreadControlBlock *array[], ThreadControlBlock *item)
    {
        for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            if (array[i] != nullptr)
                if (array[i] == item)
                    array[i] = nullptr;
    }

    void remove_thread(ThreadControlBlock *thread)
    {
        struct list_head *plist;
        free_thread_in_proc(thread->Parent->Threads, thread);
        debug("Thread %d terminated", thread->ThreadID);
        FreeStack(thread->Stack);
        kfree(thread);
    }

    void remove_process(ProcessControlBlock *process)
    {
        // TODO: add removal of child processes & and from parent child list
        if (process == nullptr)
            return;

        if (process->State == STATE_TERMINATED)
        {
            for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            {
                check_thread(process->Threads[i]);
                remove_thread(process->Threads[i]);
            }
            FreeProcess(process);
            debug("Process %d terminated", process->ProcessID);
            FreePage(process->PageTable);
            kfree(process);
        }
        else
        {
            for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            {
                check_thread(process->Threads[i]);
                if (process->Threads[i]->State == STATE_TERMINATED)
                {
                    remove_thread(process->Threads[i]);
                }
            }

            bool processisempty = true;
            for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            {
                check_thread(process->Threads[i]);
                processisempty = false;
                // TODO: check for process childs too
            }
            if (processisempty)
                remove_process(process);
        }
    }

    extern "C"
    {
        void do_exit(uint64_t code)
        {
            SILENT_LOCK(exit_lock);
            trace("parent:%s tid:%d, code:%016p", CurrentProcess->Name, CurrentThread->ThreadID, code);
            SILENT_LOCK(scheduler_lock);
            CurrentThread->State = STATE_TERMINATED;
            CurrentProcess->ExitCode = code;
            CurrentThread->ExitCode = code;
            trace("Exiting thread %d...", CurrentThread->ThreadID);
            SILENT_LOCK(scheduler_lock);
            SILENT_UNLOCK(exit_lock);
            CPU_STOP;
        }
    }

    ProcessControlBlock *create_process(ProcessControlBlock *parent, char *name)
    {
        SILENT_LOCK(process_lock);
        trace("name: %s", name);
        ProcessControlBlock *process = new ProcessControlBlock;

        process->ProcessID = upcoming_pid++;
        process->State = STATE_READY;
        process->Checksum = PROCESS_CHECKSUM;
        memcpy(process->Name, name, sizeof(process->Name));
        if (parent)
            process->Parent = parent;
        process->PageTable = CreateNewPML4();
        debug("%s address space: %#x", name, process->PageTable);

        for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
            process->Threads[i] = nullptr;

        set_cb_time(process->Time);
        AllocateProcess(process);
        trace("New thread created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", process->ProcessID, process->Time->h, process->Time->m, process->Time->s, process->Time->d, process->Time->M, process->Time->y);
        SILENT_UNLOCK(process_lock);
        return process;
    }

    ThreadControlBlock *create_thread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy)
    {
        SILENT_LOCK(thread_lock);
        if (parent->Checksum != PROCESS_CHECKSUM)
        {
            err("Thread cannot have a null parent!");
            return nullptr;
        }
        trace("fct: %016p, Priority: %d, State: %d, Policy %d | child of \"%s\"", function, Priority, State, Policy, parent->Name);
        ThreadControlBlock *thread = new ThreadControlBlock;

        thread->ThreadID = upcoming_tid++;
        thread->Parent = parent;
        thread->State = State;
        thread->Policy = Policy;
        thread->Checksum = THREAD_CHECKSUM;

        if (1) // TODO: kernel & user
        {
            thread->Stack = AllocateStack();
            thread->Registers.ds = GDT_KERNEL_DATA;
            thread->Registers.cs = GDT_KERNEL_CODE;
            thread->Registers.ss = GDT_KERNEL_DATA;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
            thread->Registers.STACK = (uint64_t)thread->Stack;
            POKE(uint64_t, thread->Registers.rsp) = (uint64_t)do_exit;
        }
        else
        {
            thread->Registers.ds = GDT_USER_DATA;
            thread->Registers.cs = GDT_USER_CODE;
            thread->Registers.ss = GDT_USER_DATA;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
        }

        thread->Registers.FUNCTION = (uint64_t)function;
        thread->Registers.ARG0 = (uint64_t)args0; // args0
        thread->Registers.ARG1 = (uint64_t)args1; // args1

        set_cb_time(thread->Time);
        alloc_thread_in_proc(parent->Threads, thread);
        trace("New thread created->TID:%d-Time:%02d:%02d:%02d    %02d.%02d.%02d", thread->ThreadID, thread->Time->h, thread->Time->m, thread->Time->s, thread->Time->d, thread->Time->M, thread->Time->y);
        SILENT_UNLOCK(thread_lock);
        return thread;
    }

    ProcessControlBlock *MultiTasking::CreateProcess(ProcessControlBlock *parent, char *name)
    {
        return create_process(parent, name);
    }
    ThreadControlBlock *MultiTasking::CreateThread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy)
    {
        return create_thread(parent, function, args0, args1, Priority, State, Policy);
    }

    void SetPageTable(VMM::PageTable *PML)
    {
        debug("Setting the new page table %016p", PML);
        if (PML->Entries->GetAddress() == 0) // TODO: check if the page table is valid
        {
            err("Address space %016p is not valid!", PML);
            return;
        }
        asm volatile("mov %[PML], %%cr3"
                     :
                     : [PML] "q"(PML)
                     : "memory");
    }

    __attribute__((noreturn)) static void IdleProcessLoop()
    {
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
        __attribute__((naked, used)) void advanced_schedule_handler_helper()
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
                "call scheduler_interrupt_handler\n"
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

        InterruptHandler(scheduler_interrupt_handler)
        {
            uint64_t timeslice = PRIORITY_MEDIUM; // cannot jump from this goto statement to its label on "goto scheduler_eoi;"
            if (!ScheduleOn)
                goto scheduler_eoi;

            SILENT_LOCK(scheduler_lock);
            ScheduleOn = false;
            if (INT_NUM != IRQ11)
                panic("Something is wrong with Scheduler (corrupted registers)");

            if (!CurrentProcess || !CurrentThread)
            {
                for (size_t i = 0; i < MAX_PROCESSES; i++)
                {
                    check_process(ListProcess[i]);
                    ProcessControlBlock *process = ListProcess[i];
                    if (process->State != STATE_READY)
                    {
                        remove_process(process);
                        continue;
                    }

                    for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
                    {
                        check_thread(process->Threads[i]);
                        ThreadControlBlock *thread = process->Threads[i];
                        if (thread->State != STATE_READY)
                            continue;
                        CurrentProcess = process;
                        CurrentThread = thread;
                        timeslice = thread->Priority;
                        // debug("[sched] Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
                        goto scheduler_success;
                    }
                }
                goto scheduler_idle;
            }
            else
            {
                CurrentThread->Registers = *regs;
                if (CurrentThread->State == STATE_RUNNING)
                    CurrentThread->State = STATE_READY;

                bool threadfound = false;
                bool processfound = false;

                for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
                {
                    check_thread(CurrentProcess->Threads[i]);
                    if (threadfound == false)
                        check_thread(CurrentProcess->Threads[(i + 1)]);
                    if (threadfound)
                    {
                        threadfound = false;
                        if (CurrentProcess->State != STATE_READY)
                            break;
                        ThreadControlBlock *thread = CurrentProcess->Threads[i];
                        if (CurrentProcess->State != STATE_READY)
                            break;
                        if (thread->State != STATE_READY)
                            continue;
                        CurrentThread = thread;
                        timeslice = CurrentThread->Priority;
                        // debug("[sched] Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
                        goto scheduler_success;
                    }
                    else
                    {
                        if (CurrentProcess->Threads[i] == CurrentThread)
                            threadfound = true;
                    }
                }

                for (size_t i = 0; i < MAX_PROCESSES; i++)
                {
                    check_process(ListProcess[i]);
                    // if (processfound == false) // unexecuted behaviour
                    //     check_process(ListProcess[(i + 1)]);

                    if (processfound)
                    {
                        processfound = false;
                        ProcessControlBlock *proc = ListProcess[i];
                        if (proc->State != STATE_READY)
                            continue;
                        for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
                        {
                            check_thread(proc->Threads[i]);
                            ThreadControlBlock *thread = proc->Threads[i];
                            if (thread->State != STATE_READY)
                                continue;
                            remove_process(CurrentProcess);
                            CurrentProcess = proc;
                            CurrentThread = thread;
                            timeslice = CurrentThread->Priority;
                            // debug("[sched] Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
                            goto scheduler_success;
                        }
                    }
                    else
                    {
                        check_process(ListProcess[(i + 1)]);
                        if (ListProcess[i] == CurrentProcess)
                            processfound = true;
                    }
                }

                size_t max_proc_tmp = 0;
                for (size_t i = 0; i < MAX_PROCESSES; i++)
                {
                    check_process(ListProcess[i]);
                    if (ListProcess[i] == CurrentProcess)
                    {
                        max_proc_tmp = i;
                        break;
                    }
                }

                for (size_t p = 0; p < max_proc_tmp + 1; p++)
                {
                    check_process(ListProcess[p]);
                    ProcessControlBlock *proc = ListProcess[p];
                    if (proc->State != STATE_READY)
                        continue;
                    for (size_t i = 0; i < PROCESS_MAX_THREADS; i++)
                    {
                        check_thread(proc->Threads[i]);
                        ThreadControlBlock *thread = proc->Threads[i];
                        if (thread->State != STATE_READY)
                            continue;
                        remove_process(CurrentProcess);
                        CurrentProcess = proc;
                        CurrentThread = thread;
                        timeslice = CurrentThread->Priority;
                        // debug("[sched] Scheduling thread %d parent of %s", thread->ThreadID, thread->Parent->Name);
                        goto scheduler_success;
                    }
                }
            }

        scheduler_idle:
            if (CurrentProcess != nullptr)
                remove_process(CurrentProcess);
            IdleProcess = create_process(nullptr, "idle");
            IdleThread = create_thread(IdleProcess, reinterpret_cast<uint64_t>(IdleProcessLoop), 0, 0, PRIORITY_VERYLOW, STATE_READY, POLICY_KERNEL);
            timeslice = IdleThread->Priority;
            CurrentProcess = IdleProcess;
            CurrentThread = IdleThread;
            *regs = IdleThread->Registers;
            SetPageTable(IdleProcess->PageTable);
            debug("[sched] Idling...");
            Yield(timeslice);
            goto scheduler_end;
        scheduler_success:
            CurrentThread->State = STATE_RUNNING;
            UpdateProcessTimeUsed(CurrentProcess->Time);
            UpdateProcessTimeUsed(CurrentThread->Time);
            *regs = CurrentThread->Registers;
            SetPageTable(CurrentProcess->PageTable);
            Yield(timeslice);
        scheduler_end:
            SILENT_UNLOCK(scheduler_lock);
            ScheduleOn = true;
        scheduler_eoi:
            EndOfInterrupt(INT_NUM);
        }
    }

    MultiTasking::MultiTasking()
    {
        trace("initializing lists for scheduler...");
        for (size_t i = 0; i < MAX_PROCESSES; i++)
            ListProcess[i] = nullptr;
        // if (mwait_available)
        // {
        //     IdleTrigger = true;
        //     // monitor((size_t)&IdleTrigger, 0, 0);
        // }
        ScheduleOn = true;
        CurrentTaskingMode = TaskingMode::Advanced;
    }

    MultiTasking::~MultiTasking()
    {
        // TODO: remove everything but idk what to say about this...
        ScheduleOn = false;
        CurrentTaskingMode = TaskingMode::None;
    }
};

void schedule()
{
    ScheduleInterrupt;
}
