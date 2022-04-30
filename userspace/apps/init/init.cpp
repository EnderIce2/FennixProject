#include <io.h>
#include <syscalls.h>

int main(int argc, char **argv)
{
    syscall_dbg(0x3F8, (char *)"This is a test so we can know if the syscalls are working as expected.\n");
    syscall_createProcess((char *)"/system/uiserver");
    syscall_createProcess((char *)"/system/wm");
    syscall_createProcess((char *)"/system/fesktop");
    while (1)
        ;
    return 0;
}
