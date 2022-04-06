#include <task.h>
#include <heap.h>
#include <debug.h>

int CurrentTaskingMode = TaskingMode::None;

using namespace PrimitiveTasking;
using namespace AdvancedTasking;

ProcessControlBlock *APICALL SysGetProcessByPID(uint64_t ID)
{
    fixme("%d", ID);
    return NULL;
}

ProcessControlBlock *APICALL SysGetCurrentProcess()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Primitive:
    {
        ProcessControlBlock *proc = nullptr;
        fixme("unimplemented for primitive tasking");
        return proc;
    }
    case TaskingMode::Advanced:
    {
        return MultiProcessing.GetCurrentProcess();
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ProcessControlBlock *APICALL SysCreateProcess(const char *Name, void *PageTable)
{
    debug("Current Tasking Mode %d", CurrentTaskingMode);
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Primitive:
    {
        ProcessControlBlock *proc = new ProcessControlBlock;
        memcpy(proc->Name, Name, sizeof(proc->Name));
        proc->PageTable = (VMM::PageTable *)PageTable;
        return proc;
    }
    case TaskingMode::Advanced:
    {
        ProcessControlBlock *proc = nullptr;
        return proc;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ThreadControlBlock *APICALL SysCreateThread(ProcessControlBlock *Parent, uint64_t InstructionPointer)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Primitive:
    {
        ThreadControlBlock *thread = new ThreadControlBlock;
        TaskControlBlock *task = SingleProcessing.CreateTask(InstructionPointer, 0, 0, Parent->Name);
        Parent->ProcessID = task->id;
        if (!Parent->PageTable)
            Parent->PageTable = (VMM::PageTable *)task->pml4;
        thread->Checksum = THREAD_CHECKSUM;
        thread->ExitCode = 0;
        thread->Parent = Parent;
        thread->Policy = POLICY_KERNEL;
        thread->Priority = PRIORITY_MEDIUM;
        thread->State = STATE_READY;
        thread->Registers = task->regs;
        thread->Stack = task->stack;
        thread->ThreadID = task->id;
        Parent->Threads[0] = thread;
        return thread;
    }
    case TaskingMode::Advanced:
    {
        ThreadControlBlock *thread = new ThreadControlBlock;
        return thread;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

// TODO: implement for primitive tasking if enabled to suspend the current task and run the created one
ProcessControlBlock *APICALL SysCreateProcessFromFile(const char *File)
{
    const char *arg = File;
    return nullptr;
}
