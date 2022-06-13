#include <internal_task.h>

#include <interrupts.h>
#include <debug.h>
#include <heap.h>
#include <asm.h>
#include <int.h>
#include <io.h>

#include "../cpu/apic.hpp"
#include "../cpu/gdt.h"
#include "../timer.h"

namespace Tasking
{
    Monotasking *monot = nullptr;

#define MAX_TASKS 0x10000
    TaskControlBlock *CurrentTask = nullptr;
    TaskControlBlock *TaskQueue[MAX_TASKS];

    extern "C"
    {
        __attribute__((naked, used)) void MonoTaskingSchedulerHelper()
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
                "call mono_schedule_interrupt_handler\n"
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
    }
#define SchedulerInterrupt IRQ17

    static uint64_t TaskIDs = 0;

    TaskControlBlock *FindLastTask()
    {
        for (int i = MAX_TASKS; i > -1; i--)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                continue;
            debug("Last task in queue is %s.", TaskQueue[i]->name);
            return TaskQueue[i];
        }
        // debug("No more tasks in queue. Returning null...");
        return nullptr;
    }

    void AllocateTask(TaskControlBlock *task)
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] != nullptr)
                continue;
            if (TaskQueue[i]->checksum == TASK_CHECKSUM)
                continue;
            TaskQueue[i] = task;
            debug("Task %s has been added to queue", task->name);
            break;
        }
    }

    void FreeTask(TaskControlBlock *task)
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                continue;
            if (TaskQueue[i] == task)
            {
                debug("Task %s will be removed from queue.", task->name);
                KernelStackAllocator->FreeStack((void *)TaskQueue[i]->stack);
                // KernelPageTableAllocator->RemovePageTable((VMM::PageTable *)TaskQueue[i]->pml4.raw);
                delete TaskQueue[i];
                memset(TaskQueue[i], 0, sizeof(TaskControlBlock));
                TaskQueue[i] = FindLastTask();
                debug("Task removed.");
                break;
            }
        }
    }

    extern "C"
    {
        InterruptHandler(mono_schedule_interrupt_handler)
        {
            foreach (auto t in TaskQueue)
            {
                if (t->state == TaskState::TaskPushed)
                {
                    t->state = TaskState::TaskStateWaiting;
                    // t->regs = *regs;
                }
                else if (t->state == TaskState::TaskPoped)
                {
                    t->state = TaskState::TaskStateWaiting;
                    // *regs = t->regs;
                }
            }

            bool task_changed = false;
            for (int i = MAX_TASKS; i > -1; i--)
            {
                if (TaskQueue[i] == nullptr)
                    continue;
                if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                    continue;
                switch (TaskQueue[i]->state)
                {
                case TaskState::TaskStateReady:
                {
                    TaskQueue[i]->state = TaskState::TaskStateRunning;
                    *regs = TaskQueue[i]->regs;
                    writecr3(TaskQueue[i]->pml4);
                    CurrentTask = TaskQueue[i];
                    trace("Task %s is now running.", TaskQueue[i]->name);
                    task_changed = true;
                    goto scheduler_eoi;
                }
                case TaskState::TaskStateWaiting:
                {
                    debug("Task %s is waiting for another task.", TaskQueue[i]->name);
                    continue;
                }
                case TaskState::TaskStateTerminated:
                {
                    FreeTask(TaskQueue[i]);
                    if (FindLastTask() == nullptr)
                    {
                        err("No more tasks to run! System halted."); // we never want to get here.
                        CPU_STOP;
                    }
                    continue;
                }
                case TaskState::TaskPushed:
                case TaskState::TaskPoped:
                case TaskState::TaskStateRunning:
                default:
                    continue;
                }
            }

        scheduler_eoi:
            EndOfInterrupt(INT_NUM);
            if (!task_changed)
                apic->OneShot(SchedulerInterrupt, 100);
        }
    }

    __attribute__((__noreturn__)) void MonoTaskingTaskExit()
    {
        CurrentTask->state = TaskState::TaskStateTerminated;
        trace("Task %s exited.", CurrentTask->name);
        apic->OneShot(SchedulerInterrupt, 100);
        CPU_STOP;
    }

    TaskControlBlock *Monotasking::CreateTask(uint64_t InstructionPointer, uint64_t FirstArgument, uint64_t SecondArgument, char *Name, bool UserMode)
    {
        TaskControlBlock *task = new TaskControlBlock;
        task->id = TaskIDs++;
        memcpy(((char *)task->name), Name, sizeof(task->name));
        task->checksum = TASK_CHECKSUM;
        task->UserMode = UserMode;
        task->state = TaskState::TaskStateReady;
        task->stack = KernelStackAllocator->AllocateStack(UserMode);
        task->pml4 = KernelPageTableAllocator->CreatePageTable(UserMode);

        memset(&task->regs, 0, sizeof(TrapFrame));
        if (!UserMode)
        {
            // task->regs.ds = GDT_KERNEL_DATA;
            task->regs.ss = GDT_KERNEL_DATA;
            task->regs.cs = GDT_KERNEL_CODE;
            task->gs = (uint64_t)task;
            task->fs = rdmsr(MSR_FS_BASE);
            task->regs.rflags.always_one = 1;
            task->regs.rflags.IF = 1;
            task->regs.rflags.ID = 1;
            task->regs.STACK = (uint64_t)task->stack;
            POKE(uint64_t, task->regs.rsp) = (uint64_t)MonoTaskingTaskExit;
        }
        else
        {
            // task->regs.ds = GDT_USER_DATA;
            task->regs.cs = GDT_USER_CODE;
            task->regs.ss = GDT_USER_DATA;
            task->gs = 0;
            task->fs = rdmsr(MSR_FS_BASE);
            task->regs.rflags.always_one = 1;
            task->regs.rflags.IF = 1;
            task->regs.rflags.ID = 1;
            task->regs.STACK = (uint64_t)task->stack;
        }
        task->regs.FUNCTION = (uint64_t)InstructionPointer;
        task->regs.ARG0 = (uint64_t)FirstArgument;
        task->regs.ARG1 = (uint64_t)SecondArgument;

        task->SpawnTick = counter();
        uint32_t t = 0;
        outb(0x70, 0x00);
        t = inb(0x71);
        task->Second = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x02);
        t = inb(0x71);
        task->Minute = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x04);
        t = inb(0x71);
        task->Hour = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x07);
        t = inb(0x71);
        task->Day = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x08);
        t = inb(0x71);
        task->Month = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x09);
        t = inb(0x71);
        task->Year = ((t & 0x0F) + ((t >> 4) * 10));
        trace("Task %s has been created. (IP %#llx)", task->name, InstructionPointer);
        AllocateTask(task);
        return task;
    }

    void Monotasking::KillMe()
    {
        CurrentTask->state = TaskState::TaskStateTerminated;
        trace("Task %s commited suicide.", CurrentTask->name);

        for (uint64_t i = 0; i < MAX_TASKS; i++)
            if (CurrentTask == TaskQueue[i])
                if (TaskQueue[i - 1] != nullptr &&
                    TaskQueue[i - 1]->checksum == TASK_CHECKSUM)
                    this->PopTask();

        apic->OneShot(SchedulerInterrupt, 100);
        CPU_STOP;
    }

    void Monotasking::PushTask(uint64_t rip)
    {
        if (FindLastTask() == CurrentTask)
        {
            err("The current task is the last task in queue.");
            return;
        }

        for (uint64_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                continue;
            if (CurrentTask == TaskQueue[i])
            {
                if (TaskQueue[i + 1] == nullptr)
                    continue;
                if (TaskQueue[i + 1]->checksum != TASK_CHECKSUM)
                    continue;
                if (TaskQueue[i]->state != TaskState::TaskStateTerminated)
                    TaskQueue[i]->state = TaskState::TaskPushed;
                TaskQueue[i + 1]->state = TaskState::TaskStateReady;
                // TaskQueue[i + 1]->regs.rip = rip; // i need to find anoter way to get the instruction pointer
                trace("Task pushed to %s with instruction pointer %#llx.", TaskQueue[i + 1]->name, rip);
                apic->OneShot(SchedulerInterrupt, 100);
                return;
            }
        }
        err("Cannot move to the next task.");
    }

    void Monotasking::PopTask()
    {
        if (TaskQueue[0] == CurrentTask)
        {
            err("The current task is the first task in queue.");
            return;
        }

        for (uint64_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                continue;
            if (CurrentTask == TaskQueue[i])
            {
                if (TaskQueue[i - 1] == nullptr)
                    continue;
                if (TaskQueue[i - 1]->checksum != TASK_CHECKSUM)
                    continue;
                if (TaskQueue[i]->state != TaskState::TaskStateTerminated)
                    TaskQueue[i]->state = TaskState::TaskPoped;
                TaskQueue[i - 1]->state = TaskState::TaskStateReady;
                trace("Task popped to %s.", TaskQueue[i - 1]->name);
                apic->OneShot(SchedulerInterrupt, 100);
                return;
            }
        }
        err("Cannot move to the previous task.");
    }

    Monotasking::Monotasking(uint64_t FirstTask)
    {
        for (uint64_t i = 0; i < MAX_TASKS; i++)
            TaskQueue[i] = nullptr; // Make sure that all tasks have value nullptr
        CreateTask((uint64_t)FirstTask, 0, 0, (char *)"kernel", false);
        CurrentTaskingMode = TaskingMode::Mono;
        apic->RedirectIRQ(0, SchedulerInterrupt - 32, 1);
        apic->OneShot(SchedulerInterrupt, 100);
    }

    Monotasking::~Monotasking()
    {
        for (uint64_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i] == CurrentTask)
                continue; // We ignore the current task because it's the task that invoked the unitialization.
            TaskControlBlock *task = TaskQueue[i];
            trace("Task %s has been forcibly terminated and removed.", task->name);
            FreeTask(task);
            CurrentTaskingMode = TaskingMode::None;
        }
    }
};
