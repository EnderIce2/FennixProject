#include <internal_task.h>

#include <interrupts.h>
#include <debug.h>
#include <heap.h>
#include <asm.h>
#include <int.h>
#include <sys.h>
#include <io.h>

#include "../cpu/apic.hpp"
#include "../cpu/smp.hpp"
#include "../cpu/gdt.h"
#include "../timer.h"

// #define DEBUG_TASK_MANAGER 1

#ifdef DEBUG_TASK_MANAGER
#include <critical.hpp>
#include <display.h>
#endif

namespace Tasking
{
    MonoTasking *monot = nullptr;

// IRQ17 = 0x31
#define SchedulerInterrupt __asm__ volatile("int $0x31")

    struct TaskQueue
    {
        TaskQueue *Prev = nullptr;
        TaskControlBlock *Task = nullptr;
        TaskQueue *Next = nullptr;
    };

    TaskQueue *Queue = nullptr;
    TaskControlBlock *CurrentTask = nullptr;

#ifdef DEBUG_TASK_MANAGER
    void drawrectangle(uint64_t X, uint64_t Y, uint64_t W, uint64_t H, uint32_t C)
    {
        for (uint64_t y = Y; y < Y + H; y++)
            for (uint64_t x = X; x < X + W; x++)
                CurrentDisplay->SetPixel(x, y, C);
    }

    InterruptHandler(TraceSchedOnScreen)
    {
        EnterCriticalSection;
        inb(0x60);
        int offset = 0;
        TaskQueue *current = Queue;
        CurrentDisplay->ResetPrintColor();

        do
        {
            int color = 0x255AFF;
            switch (current->Task->state)
            {
            case TaskState::TaskStateReady:
                color = 0x0FFFF0;
                break;
            case TaskState::TaskStateRunning:
                color = 0x0AAF00;
                break;
            case TaskState::TaskStateWaiting:
                color = 0xF75F00;
                break;
            case TaskState::TaskStateTerminated:
                color = 0xFF0000;
                break;
            case TaskState::TaskPushed:
                color = 0x5000FF;
                break;
            case TaskState::TaskPoped:
                color = 0xA00AF0;
                break;
            default:
                break;
            }
            drawrectangle(0, offset, 50, 10, color);
            CurrentDisplay->SetPrintLocation(0, offset);
            printf("%s", current->Task->name);
            offset += 10;
            current = current->Next;
        } while (current);
        LeaveCriticalSection;
    }
#endif

    static uint64_t TaskIDs = 0;

    TaskControlBlock *FindLastTask()
    {
        TaskQueue *current = Queue;
        while (current->Next)
            current = current->Next;
        if (!current->Task)
        {
            debug("Task is null");
            return nullptr;
        }
        debug("Last task in queue is %s.", current->Task->name);
        return current->Task;
    }

    void AllocateTask(TaskControlBlock *task, bool AfterCurrent)
    {
        if (!Queue)
        {
            Queue = new TaskQueue;
            Queue->Task = task;
            Queue->Next = nullptr;
            Queue->Prev = nullptr;
            return;
        }

        TaskQueue *newTask = new TaskQueue;
        newTask->Task = task;

        if (AfterCurrent)
        {
            TaskQueue *current = Queue;

            while (current->Task != CurrentTask)
            {
                if (!current)
                    break;
                current = current->Next;
            }

            newTask->Next = current->Next;
            newTask->Prev = current;
            current->Next = newTask;
        }
        else
        {
            TaskQueue *last = Queue;
            while (last->Next)
                last = last->Next;

            newTask->Prev = last;
            newTask->Next = nullptr;
            last->Next = newTask;
        }
    }

    void FreeTask(TaskControlBlock *task)
    {
        if (!Queue)
        {
            if (task)
                delete task;
            return;
        }

        TaskQueue *current = Queue;
        while (current)
        {
            if (current->Task == task)
            {
                if (current->Next)
                {
                    TaskQueue *next = current->Next;
                    current->Task = next->Task;
                    current->Next = next->Next;
                    if (next->Next)
                        next->Next->Prev = current;
                    delete next;
                }
                else
                {
                    TaskQueue *prev = current->Prev;
                    delete current;
                    prev->Next = nullptr;
                }
                break;
            }
            current = current->Next;
        }
        delete task;
    }

    extern "C"
    {
        __attribute__((naked, used)) void MonoTaskingSchedulerHelper()
        {
#if defined(__amd64__)
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
                "movq %rsp, %rdi\n"
                "call MonoSchedulerHelperHandler\n"
                "popq %rax\n"
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
#elif defined(__i386__)
            asm("cld\n"
                "pusha\n"
                "movl %esp, %edi\n"
                "call MonoSchedulerHelperHandler\n"
                "popa\n"
                "addl $16, %esp\n"
                "iret");
#endif
        }

        InterruptHandler(MonoSchedulerHelperHandler)
        {
            debug("Mono scheduler called.");
            TaskQueue *current = Queue;
            do
            {
                if (current->Task->state == TaskState::TaskPushed)
                {
                    current->Task->state = TaskState::TaskStateWaiting;
                    // current->Task->regs = *regs;
                }
                else if (current->Task->state == TaskState::TaskPoped)
                {
                    current->Task->state = TaskState::TaskStateWaiting;
                    // *regs = current->Task->regs;
                }
                current = current->Next;
            } while (current);

            current = Queue;

            while (current->Next)
                current = current->Next;

            bool TaskChanged = false;

            do
            {
                if (current->Task->checksum == TASK_CHECKSUM)
                {
                    switch (current->Task->state)
                    {
                    case TaskState::TaskStateReady:
                    {
                        current->Task->state = TaskState::TaskStateRunning;
                        *regs = current->Task->regs;
                        asm volatile("mov %[ControlRegister], %%cr3"
                                     :
                                     : [ControlRegister] "q"(current->Task->pml4)
                                     : "memory");
                        CurrentCPU->PageTable.raw = (uint64_t)current->Task->pml4;
                        CurrentTask = current->Task;
                        trace("Task %s is now running. [RIP:%#lx PML:%#lx]",
                              current->Task->name, current->Task->regs.rip, current->Task->pml4);
                        TaskChanged = true;
                        goto scheduler_eoi;
                    }
                    case TaskState::TaskStateWaiting:
                    {
                        trace("Task %s is waiting for another task.", current->Task->name);
                        break;
                    }
                    case TaskState::TaskStateTerminated:
                    {
                        FreeTask(current->Task);
                        if (FindLastTask() == nullptr)
                            panic("No more tasks to run! System halted.", false); // we never want to get here.
                        break;
                    }
                    case TaskState::TaskPushed:
                    case TaskState::TaskPoped:
                    case TaskState::TaskStateRunning:
                    default:
                        break;
                    }
                }
                current = current->Prev;
            } while (current);

        scheduler_eoi:
            EndOfInterrupt(INT_NUM);
            if (!TaskChanged)
                SchedulerInterrupt;
        }
    }

    __attribute__((__noreturn__)) void MonoTaskingTaskExit()
    {
        CurrentTask->state = TaskState::TaskStateTerminated;
        trace("Task %s exited.", CurrentTask->name);
        SchedulerInterrupt;
        CPU_STOP;
    }

    void MonoTasking::SetTaskTimeInfo(TaskControlBlock *task)
    {
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
    }

    TaskControlBlock *MonoTasking::CreateTask(uint64_t InstructionPointer, uint64_t FirstArgument, uint64_t SecondArgument, char *Name, bool UserMode, bool AfterCurrent)
    {
        TaskControlBlock *task = new TaskControlBlock;
        task->id = TaskIDs++;
        strcpy(task->name, Name);
        task->checksum = TASK_CHECKSUM;
        task->UserMode = UserMode;
        task->state = TaskState::TaskStateReady;
        task->stack = KernelStackAllocator->AllocateStack(UserMode);
        task->pml4 = (VMM::PageTable *)KernelPageTableAllocator->CreatePageTable(UserMode).raw;

        memset(&task->regs, 0, sizeof(TrapFrame));
        if (!UserMode)
        {
            task->regs.cs = GDT_KERNEL_CODE;
            task->regs.ds = GDT_KERNEL_DATA;
            task->regs.ss = GDT_KERNEL_DATA;
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
            task->regs.cs = GDT_USER_CODE;
            task->regs.ds = GDT_USER_DATA;
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
        task->argc = FirstArgument;
        task->argv = (char **)SecondArgument;
        this->SetTaskTimeInfo(task);
        trace("Task %s has been created. (IP %#llx)", task->name, InstructionPointer);
        AllocateTask(task, AfterCurrent);
        return task;
    }

    void MonoTasking::KillMe()
    {
        CurrentTask->state = TaskState::TaskStateTerminated;
        trace("Task %s killed.", CurrentTask->name);
        this->PopTask(true);
        SchedulerInterrupt;
        CPU_STOP;
    }

    void MonoTasking::PushTask(uint64_t rip)
    {
        if (FindLastTask() == CurrentTask)
        {
            err("The current task is the last task in queue.");
            return;
        }

        TaskQueue *current = Queue;
        do
        {
            if (CurrentTask == current->Task)
            {
                if (current->Task->state != TaskState::TaskStateTerminated)
                    current->Task->state = TaskState::TaskPushed;
                if (current->Next)
                    current->Next->Task->state = TaskState::TaskStateReady;

                // if (rip != 0)
                //     CurrentTask->regs.rip = rip; // FIXME: i need to find anoter way to get the instruction pointer
                trace("Task pushed to %s with instruction pointer %#llx.", current->Task->name, rip);
                SchedulerInterrupt;
            }
            current = current->Next;
        } while (current);
        err("Cannot move to the next task.");
    }

    void MonoTasking::PopTask(bool Destroy)
    {
        if (Queue->Task == CurrentTask)
        {
            err("The current task is the first task in queue.");
            return;
        }

        TaskQueue *current = Queue;
        do
        {
            if (CurrentTask == current->Task)
            {
                if (!current->Prev)
                {
                    err("Cannot move to the previous task.");
                    return;
                }
                else
                    current->Prev->Task->state = TaskState::TaskStateReady;

                if (Destroy)
                {
                    FreeTask(current->Task);
                    trace("Task %s destroyed.", current->Task->name);
                    return;
                }
                else if (current->Task->state != TaskState::TaskStateTerminated)
                    current->Task->state = TaskState::TaskPoped;
                else
                {
                    err("Cannot move to the previous task.");
                    return;
                }

                trace("Task poped from %s.", current->Task->name);
                SchedulerInterrupt;
            }
            current = current->Next;
        } while (current);
    }

    TaskControlBlock *MonoTasking::GetCurrentTask() { return CurrentTask; }

    MonoTasking::MonoTasking(uint64_t FirstTask)
    {
        CreateTask((uint64_t)FirstTask, 0, 0, (char *)"kernel", false, true);
        CurrentTaskingMode = TaskingMode::Mono;
        // apic->RedirectIRQ(0, SchedulerInterrupt - 32, 1);
#ifdef DEBUG_TASK_MANAGER
        RegisterInterrupt(TraceSchedOnScreen, IRQ12, true); // temporarily workaround
#endif
        SchedulerInterrupt;
    }

    MonoTasking::~MonoTasking()
    {
        TaskQueue *current = Queue;

        while (1)
        {
            if (!current)
                break;
            if (current->Task == CurrentTask)
                continue;
            trace("Task %s has been forcibly terminated and removed.", current->Task->name);
            FreeTask(current->Task);
            delete current;
            current = current->Next;
        }
        CurrentTaskingMode = TaskingMode::None;
    }
};
