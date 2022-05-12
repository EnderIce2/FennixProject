#include <internal_task.h>
#include <interrupts.h>
#include <int.h>
#include <heap.h>
#include <debug.h>
#include <asm.h>
#include "../cpu/gdt.h"

namespace Tasking
{
    Monotasking *monot = nullptr;

    extern "C"
    {
        __attribute__((naked, used)) void mono_scheduler_handler_helper()
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

#define ScheduleInterrupt asm volatile("int $0x2a")
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
                KernelPageTableAllocator->RemovePageTable((VMM::PageTable *)TaskQueue[i]->pml4);
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
            bool task_changed = false;
            for (size_t i = 0; i < MAX_TASKS; i++)
            {
                if (TaskQueue[i] == nullptr)
                    goto scheduler_eoi; // We are at the end of the queue (at FreeTask we get the last added task and replace with the removed one so we don't have to search through the array)
                if (TaskQueue[i]->checksum != TASK_CHECKSUM)
                    goto scheduler_eoi;
                if (TaskQueue[i]->state == TaskState::TaskStateWaiting)
                {
                    TaskQueue[i]->state = TaskState::TaskStateRunning;
                    *regs = TaskQueue[i]->regs;
                    CR3 CR3PageTable;
                    CR3PageTable.raw = (uint64_t)TaskQueue[i]->pml4;
                    writecr3(CR3PageTable);
                    CurrentTask = TaskQueue[i];
                    trace("Task %s is now running.", TaskQueue[i]->name);
                    task_changed = true;
                    goto scheduler_eoi;
                }
                else if (TaskQueue[i]->state == TaskState::TaskStateTerminated)
                {
                    FreeTask(TaskQueue[i]);
                    if (TaskQueue[i] == nullptr)
                        while (1)
                        {
                            asm volatile("hlt");
                            TaskControlBlock *task = FindLastTask();
                            if (task != nullptr)
                                break;
                        }
                }
                else
                    continue;
            }
        scheduler_eoi:
            EndOfInterrupt(INT_NUM);
            if (!task_changed)
                ScheduleInterrupt;
        }
    }

    __attribute__((__noreturn__)) void MonoTaskingTaskExit()
    {
        CurrentTask->state = TaskState::TaskStateTerminated;
        trace("Task %s exited.", CurrentTask->name);
        ScheduleInterrupt;
        CPU_STOP;
    }

    TaskControlBlock *Monotasking::CreateTask(uint64_t InstructionPointer, uint64_t FirstArgument, uint64_t SecondArgument, char *Name, bool UserMode)
    {
        TaskControlBlock *task = new TaskControlBlock;
        task->id = TaskIDs++;
        memcpy(((char *)task->name), Name, sizeof(task->name));
        task->checksum = TASK_CHECKSUM;
        task->UserMode = UserMode;
        task->state = TaskState::TaskStateWaiting;
        task->stack = KernelStackAllocator->AllocateStack();
        task->pml4 = KernelPageTableAllocator->CreatePageTable();
        debug("PML4 %016p for %s has been created.", task->pml4, task->name);

        if (UserMode)
        {
            fixme("User mode is not implemented yet.");
            task->UserMode = false;
        }

        memset(&task->regs, 0, sizeof(REGISTERS));
        task->regs.ds = GDT_KERNEL_DATA;
        task->regs.ss = GDT_KERNEL_DATA;
        task->regs.cs = GDT_KERNEL_CODE;
        task->regs.rflags.always_one = 1;
        task->regs.rflags.IF = 1;
        task->regs.rflags.ID = 1;
        task->regs.STACK = (uint64_t)task->stack;
        task->regs.FUNCTION = (uint64_t)InstructionPointer;
        task->regs.ARG0 = (uint64_t)FirstArgument;
        task->regs.ARG1 = (uint64_t)SecondArgument;
        POKE(uint64_t, task->regs.rsp) = (uint64_t)MonoTaskingTaskExit;

        trace("Task %s has been created. (IP %#llx)", task->name, InstructionPointer);
        AllocateTask(task);
        return task;
    }

    Monotasking::Monotasking(uint64_t FirstTask)
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
            TaskQueue[i] = nullptr; // Make sure that all tasks have value nullptr
        CreateTask((uint64_t)FirstTask, 0, 0, (char *)"kernel", false);
        CurrentTaskingMode = TaskingMode::Mono;
        ScheduleInterrupt;
    }

    Monotasking::~Monotasking()
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
        {
            if (TaskQueue[i] == nullptr)
                continue;
            if (TaskQueue[i] == CurrentTask)
                continue; // We ignore the current task because it's finit or the task that invoked the unitialization.
            TaskControlBlock *task = TaskQueue[i];
            trace("Task %s has been forcibly terminated and removed.", task->name);
            FreeTask(task);
            CurrentTaskingMode = TaskingMode::None;
        }
    }
};
