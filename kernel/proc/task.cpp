#include <task.h>
#include <display.h>
#include <debug.h>
#include <asm.h>
#include <cputables.h>
#include <bootscreen.h>
#include "../drivers/serial.h"

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};

void StartTasking(uint64_t Address, TaskingMode Mode)
{
    CLI;
    trace("Initializing Syscalls...");
    init_syscalls();
    BS->IncreaseProgres();
    trace("Starting tasking mode %d", Mode);
    switch (Mode)
    {
    case TaskingMode::Mono:
    {
        MonoTasking::SingleProcessing = new MonoTasking::MonoTasking(Address);
        BS->IncreaseProgres();
        break;
    }
    case TaskingMode::Multi:
    {
        MultiTasking::MultiProcessing = new MultiTasking::MultiTasking();
        MultiTasking::MultiProcessing->CreateThread(MultiTasking::MultiProcessing->CreateProcess(nullptr, (char *)"kernel"),
                                                    Address, 0, 0,
                                                    ControlBlockPriority::PRIORITY_REALTIME,
                                                    ControlBlockState::STATE_READY,
                                                    ControlBlockPolicy::POLICY_KERNEL, false);
        MultiTasking::MultiProcessing->ToggleScheduler(true);
        BS->IncreaseProgres();
        break;
    }
    default:
    {
        STI;
        throw;
        break;
    }
    }
    STI;
}
