#include <internal_task.h>

#include <filesystem.h>
#include <critical.hpp>
#include <msexec.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "../cpu/smp.hpp"
#include "binary.hpp"

using namespace Tasking;
using namespace FileSystem;

int CurrentTaskingMode = TaskingMode::None;

PCB *ConvertTaskCBToPCB(TaskControlBlock *task)
{
    CR3 tmpcr3;
    tmpcr3.raw = (uint64_t)task->pml4;

    static PCB pcb = {
        .ID = task->id,
        .Name = {'\0'},
        .Status = CBStatus::UnknownStatus,
        .Elevation = task->UserMode ? CBElevation::User : CBElevation::Kernel,
        .ExitCode = 0,
        .Offset = 0,
        .Parent = nullptr,
        .PageTable = tmpcr3,
        .Info = {},
        .Security = {},
        .IPCHandles = nullptr,
        .Threads = {},
        .Children = {},
        .Checksum = PROCESS_CHECKSUM};
    memcpy(pcb.Name, task->name, sizeof(pcb.Name));
    return &pcb;
}

TCB *ConvertTaskCBToTCB(TaskControlBlock *task)
{
    static TCB tcb = {
        .ID = task->id,
        .Name = {'\0'},
        .Status = CBStatus::UnknownStatus,
        .ExitCode = 0,
        .Parent = nullptr,
        .Stack = 0x0,
        .FXRegion = 0,
        .Registers = {},
        .fs = 0,
        .gs = 0,
        .cs = 0,
        .ss = 0,
        .ds = 0,
        .es = 0,
        .argc = task->argc,
        .argv = task->argv,
        .envp = task->envp,
        .Info = {},
        .Security = {},
        .Checksum = THREAD_CHECKSUM};
    memcpy(tcb.Name, task->name, sizeof(tcb.Name));
    return &tcb;
}

PCB *SysGetProcessByPID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        static PCB pcb = {
            .ID = 0,
            .Name = {'\0'},
            .Status = CBStatus::UnknownStatus,
            .Elevation = CBElevation::UnknownElevation,
            .ExitCode = 0,
            .Offset = 0,
            .Parent = nullptr,
            .PageTable = {},
            .Info = {},
            .Security = {},
            .IPCHandles = nullptr,
            .Threads = {},
            .Children = {},
            .Checksum = PROCESS_CHECKSUM};
        memcpy(pcb.Name, "Unknown Task", sizeof(pcb.Name));
        return &pcb;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (PCB *pcb in mt->ListProcess)
        {
            if (pcb == nullptr || pcb->Checksum != Checksum::PROCESS_CHECKSUM || pcb->Elevation == CBElevation::Idle)
                continue;
            if (pcb->ID == ID)
            {
                LeaveCriticalSection;
                return pcb;
            }
        }
        LeaveCriticalSection;
        return nullptr;
    }
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

TCB *SysGetThreadByTID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        static TCB tcb = {
            .ID = 0,
            .Name = {'\0'},
            .Status = CBStatus::UnknownStatus,
            .ExitCode = 0,
            .Parent = nullptr,
            .Stack = 0x0,
            .FXRegion = 0,
            .Registers = {},
            .fs = 0,
            .gs = 0,
            .cs = 0,
            .ss = 0,
            .ds = 0,
            .es = 0,
            .Info = {},
            .Security = {},
            .Checksum = THREAD_CHECKSUM};
        memcpy(tcb.Name, "Unknown Task", sizeof(tcb.Name));
        return &tcb;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (PCB *pcb in mt->ListProcess)
        {
            if (pcb == nullptr || pcb->Checksum != Checksum::PROCESS_CHECKSUM || pcb->Elevation == CBElevation::Idle)
                continue;

            foreach (TCB *tcb in pcb->Threads)
            {
                if (tcb->ID == ID)
                {
                    LeaveCriticalSection;
                    return tcb;
                }
            }
        }
        LeaveCriticalSection;
        return nullptr;
    }
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

PCB *SysGetCurrentProcess()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
        return ConvertTaskCBToPCB(monot->GetCurrentTask());
    case TaskingMode::Multi:
        return CurrentCPU->CurrentProcess;
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

TCB *SysGetCurrentThread()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
        return ConvertTaskCBToTCB(monot->GetCurrentTask());
    case TaskingMode::Multi:
        return CurrentCPU->CurrentThread;
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

void SysSetProcessPriority(int Priority)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        trace("Priority change not supported in current tasking mode. Ignoring...");
        return;
    }
    case TaskingMode::Multi:
        CurrentCPU->CurrentProcess->Info.Priority = Priority;
        return;
    default:
        return;
    }
}

int SysGetProcessPriority()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        trace("Priority get not supported in current tasking mode. Ignoring...");
        return 0;
    }
    case TaskingMode::Multi:
        return CurrentCPU->CurrentProcess->Info.Priority;
    default:
        return 0;
    }
}

void SysSetThreadPriority(int Priority)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        trace("Priority change not supported in current tasking mode. Ignoring...");
        return;
    }
    case TaskingMode::Multi:
        CurrentCPU->CurrentThread->Info.Priority = Priority;
        return;
    default:
        return;
    }
}

int SysGetThreadPriority()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        trace("Priority get not supported in current tasking mode. Ignoring...");
        return 0;
    }
    case TaskingMode::Multi:
        return CurrentCPU->CurrentThread->Info.Priority;
    default:
        return 0;
    }
}

PCB *SysCreateProcess(const char *Name, CBElevation Elevation)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        static PCB pcb = {
            .ID = 0,
            .Name = {'\0'},
            .Status = CBStatus::UnknownStatus,
            .Elevation = CBElevation::UnknownElevation,
            .ExitCode = 0,
            .Offset = 0,
            .Parent = nullptr,
            .PageTable = {},
            .Info = {},
            .Security = {},
            .IPCHandles = nullptr,
            .Threads = {},
            .Children = {},
            .Checksum = PROCESS_CHECKSUM};
        memcpy(pcb.Name, "Unknown Task", sizeof(pcb.Name));
        return &pcb;
    }
    case TaskingMode::Multi:
        return mt->CreateProcess(SysGetCurrentProcess(), (char *)Name, Elevation);
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

TCB *SysCreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t arg0, uint64_t arg1)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        static TCB tcb = {
            .ID = 0,
            .Name = {'\0'},
            .Status = CBStatus::UnknownStatus,
            .ExitCode = 0,
            .Parent = nullptr,
            .Stack = 0x0,
            .FXRegion = 0,
            .Registers = {},
            .fs = 0,
            .gs = 0,
            .cs = 0,
            .ss = 0,
            .ds = 0,
            .es = 0,
            .Info = {},
            .Security = {},
            .Checksum = THREAD_CHECKSUM};
        memcpy(tcb.Name, "Unknown Task", sizeof(tcb.Name));
        return &tcb;
    }
    case TaskingMode::Multi:
        return mt->CreateThread(Parent, InstructionPointer, arg0, arg1);
    default:
        err("Tasking mode it's disabled.");
        return nullptr;
    }
}

// TODO: implement for primitive tasking if enabled to suspend the current task and run the created one
PCB *SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, CBElevation Elevation)
{
    return ExecuteBinary(File, arg0, arg1, Elevation);
}
