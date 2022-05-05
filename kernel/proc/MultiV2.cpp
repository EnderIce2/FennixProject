#include <internal_task.h>

namespace MultiTaskingV2
{
    ProcessControlBlock *CreateProcess(ProcessControlBlock *Parent, char *Name)
    {
    }

    ThreadControlBlock *CreateThread(ProcessControlBlock *Parent,
                                     uint64_t InstructionPointer,
                                     uint64_t Arg0, uint64_t Arg1,
                                     enum ControlBlockPolicy Policy = POLICY_KERNEL,
                                     enum ControlBlockPriority Priority = PRIORITY_MEDIUM,
                                     enum ControlBlockState State = STATE_READY)
    {
    }

    MultiTaskingV2::MultiTaskingV2()
    {
    }

    MultiTaskingV2::~MultiTaskingV2()
    {
    }
}
