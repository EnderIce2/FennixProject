#include <system.h>

#include <syscalls.h>
#include "../printf.h"

char syswritedbgbuf[2048];

static inline void append_syswritedbg(char s[], char n)
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;

    int len = i;
    s[len] = n;
    s[len + 1] = '\0';
}

static inline void syswritedbgprint_wrapper(char c, void *unused)
{
    append_syswritedbg(syswritedbgbuf, c);
    (void)unused;
}

void WriteSysDebugger(const char *Format, ...)
{
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 2)
    {
        // write info about the current thread
    }
    syswritedbgbuf[0] = '\0';
    va_list args;
    va_start(args, Format);
    vfctprintf(syswritedbgprint_wrapper, NULL, Format, args);
    va_end(args);
    syscall_dbg(0x3F8, (char *)syswritedbgbuf);
}

void usleep(unsigned long Microseconds)
{
    syscall_usleep(Microseconds);
}

int system(const char *Command)
{
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 2)
        return syscall_createProcess(Command, 0, 0);
    else if (syscall_getScheduleMode() == 1)
        return -1;
}

void Exit(int Status)
{
    syscall_exit(Status);
}
