#include <internal_task.h>
#include <display.h>
#include <debug.h>
#include <asm.h>
#include <cputables.h>
#include <bootscreen.h>
#include "../drivers/serial.h"

using namespace Tasking;

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
        monot = new MonoTasking(Address);
        BS->IncreaseProgres();
        break;
    }
    case TaskingMode::Multi:
    {
        mt = new Multitasking;
        mt->CreateThread(mt->CreateProcess(nullptr, (char *)"kernel", ELEVATION::Kernel), Address, 0, 0);
        MultitaskingSchedulerEnabled = true;
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
