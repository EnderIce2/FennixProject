#include <io.h>
#include <syscalls.h>
#include <task.h>

int main(int argc, char **argv)
{
    syscall_dbg(0x3F8, (char *)"This is a test so we can know if the syscalls are working as expected.\n");
    ProcessControlBlock *uiserver = (ProcessControlBlock *)syscall_createProcess((char *)"/system/uiserver", 0, 0);
    ProcessControlBlock *windowmanager = (ProcessControlBlock *)syscall_createProcess((char *)"/system/wm", 0, 0);
    ProcessControlBlock *desktopenvironment = (ProcessControlBlock *)syscall_createProcess((char *)"/system/fesktop", 0, 0);
    return 0;
}
