#include <io.h>
#include <syscalls.h>

int main(int argc, char **argv)
{
    syscall_dbg(0x3F8, (char *)"[INIT] This is a test so we can know if the syscalls are working as expected.\n");
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 1)
    {
        syscall_dbg(0x3F8, (char *)"[INIT] Kernel is running under monotasking mode.\n");
        /* ... do stuff ... */
        syscall_createProcess((char *)"/system/monoton", 0, 0);
        syscall_pushTask();
        syscall_dbg(0x3F8, (char *)"[INIT] The shell exited!\n");
        // TODO: cleanup everything and shutdown beacuse this process should not be killed
        return 0;
    }
    else
    {
        syscall_dbg(0x3F8, (char *)"[INIT] Kernel is running under multitasking mode.\n");
        /* ... do stuff ... */
        syscall_createProcess((char *)"/system/uiserver", 0, 0);

        while (1)
            ;
    }
    return 1;
}
