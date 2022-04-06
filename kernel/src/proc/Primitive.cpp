#include <task.h>
#include <interrupts.h>
#include <heap.h>
#include <debug.h>
#include <asm.h>
#include "../cpu/gdt.h"

namespace PrimitiveTasking
{
    MonoTasking SingleProcessing = NULL;

    extern "C"
    {
        __attribute__((naked, used)) void primitive_scheduler_handler_helper()
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
                "call primitive_schedule_interrupt_handler\n"
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
        debug("No more tasks in queue. Returning null...");
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
                FreeStack((void *)TaskQueue[i]->stack);
                FreePage(TaskQueue[i]->pml4);
                delete TaskQueue[i];
                TaskQueue[i] = nullptr; // just to be sure that the garbage will not show that the task is still alive.
                TaskQueue[i] = FindLastTask();
                debug("Task removed.");
                break;
            }
        }
    }

    extern "C"
    {
        InterruptHandler(primitive_schedule_interrupt_handler)
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
                    writecr3((uint64_t)TaskQueue[i]->pml4);
                    CurrentTask = TaskQueue[i];
                    trace("Task %s is now running.", TaskQueue[i]->name);
                    task_changed = true;
                    goto scheduler_eoi;
                }
                else if (TaskQueue[i]->state == TaskState::TaskStateTerminated)
                    FreeTask(TaskQueue[i]);
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
    }

    TaskControlBlock *MonoTasking::CreateTask(uint64_t rip, uint64_t args0, uint64_t args1, char *name)
    {
        TaskControlBlock *task = new TaskControlBlock;
        task->id = TaskIDs++;
        memcpy(((char *)task->name), name, sizeof(task->name));
        task->checksum = TASK_CHECKSUM;
        task->state = TaskState::TaskStateWaiting;
        task->stack = AllocateStack();
        task->pml4 = CreateNewPML4();
        debug("PML4 %016p for %s has been created.", task->pml4, task->name);

        task->regs.ds = GDT_KERNEL_DATA;
        task->regs.ss = GDT_KERNEL_DATA;
        task->regs.cs = GDT_KERNEL_CODE;
        task->regs.rflags.always_one = 1;
        task->regs.rflags.IF = 1;
        task->regs.rflags.ID = 1;
        task->regs.STACK = (uint64_t)task->stack;
        task->regs.FUNCTION = (uint64_t)rip;
        task->regs.ARG0 = (uint64_t)args0;
        task->regs.ARG1 = (uint64_t)args1;
        POKE(uint64_t, task->regs.rsp) = (uint64_t)MonoTaskingTaskExit;

        trace("Task %s has been created. (IP %016p)", task->name, rip);
        AllocateTask(task);
        return task;
    }

    MonoTasking::MonoTasking(uint64_t firstThread)
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
            TaskQueue[i] = nullptr; // Make sure that all tasks have value nullptr
        CreateTask((uint64_t)firstThread, 0, 0, (char *)"main");
        CurrentTaskingMode = TaskingMode::Primitive;
        ScheduleInterrupt;
    }

    MonoTasking::~MonoTasking()
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