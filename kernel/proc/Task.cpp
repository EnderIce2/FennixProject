#include <internal_task.h>
#include <display.h>
#include <debug.h>
#include <asm.h>
#include <cputables.h>
#include "../drivers/serial.h"

using namespace Tasking;

void StartTasking(uint64_t Address)
{
    CLI;
    trace("Initializing Syscalls...");
    init_syscalls();

    mt = new Multitasking;
    mt->CreateThread(mt->CreateProcess(nullptr, (char *)"kernel", ELEVATION::Kernel), Address, 0, 0);
    MultitaskingSchedulerEnabled = true;
    STI;
}
