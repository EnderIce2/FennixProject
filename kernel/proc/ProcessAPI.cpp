#include <task.h>
#include <heap.h>
#include <debug.h>

using namespace MonoTasking;
using namespace MultiTasking;

int CurrentTaskingMode = TaskingMode::None;

ProcessControlBlock *APICALL SysGetProcessByPID(uint64_t ID)
{
    fixme("%d", ID);
    return NULL;
}

ProcessControlBlock *APICALL SysGetCurrentProcess()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ProcessControlBlock *proc = nullptr;
        fixme("unimplemented for primitive tasking");
        return proc;
    }
    case TaskingMode::Multi:
    {
        return MultiProcessing->GetCurrentProcess();
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
    case TaskingMode::Mono:
    {
        ProcessControlBlock *proc = new ProcessControlBlock;
        memcpy(proc->Name, Name, sizeof(proc->Name));
        if (PageTable != nullptr)
            proc->PageTable = (VMM::PageTable *)PageTable;
        else
            proc->PageTable = KernelPageTableAllocator->NewPageTable();
        return proc;
    }
    case TaskingMode::Multi:
    {
        ProcessControlBlock *proc = MultiProcessing->CreateProcess(SysGetCurrentProcess(), (char *)Name);
        if (PageTable != nullptr)
        {
            KernelAllocator.FreePage(proc->PageTable);
            proc->PageTable = (VMM::PageTable *)PageTable;
        }
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
    case TaskingMode::Mono:
    {
        ThreadControlBlock *thread = new ThreadControlBlock;
        TaskControlBlock *task = SingleProcessing->CreateTask(InstructionPointer, 0, 0, Parent->Name);
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
    case TaskingMode::Multi:
    {
        ThreadControlBlock *thread = MultiProcessing->CreateThread(Parent, InstructionPointer, 0, 0, ControlBlockPriority::PRIORITY_MEDIUM,
                                                                   ControlBlockState::STATE_READY, ControlBlockPolicy::POLICY_KERNEL);
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
ProcessControlBlock *APICALL SysCreateProcessFromFile(const char *File, bool usermode)
{
    const char *arg = File;
    return nullptr;
}