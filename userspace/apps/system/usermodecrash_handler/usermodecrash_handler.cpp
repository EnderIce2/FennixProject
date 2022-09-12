#include <io.h>
#include <syscalls.h>

#include "usm.h"

int main(uint64_t *arg1, uint64_t *arg2)
{
    syscall_dbg(0x3F8, (char *)"[UMC] Crash\n");
    TrapFrame *frame = (TrapFrame*)arg1;
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 1)
    {
        syscall_dbg(0x3F8, (char *)"[UMC] Kernel is running under monotasking mode.\n");
        return 0;
    }
    else
    {
        syscall_dbg(0x3F8, (char *)"[UMC] Kernel is running under multitasking mode.\n");
        /* Run as service */
        while (1)
            ;
    }
    return 1;
}
