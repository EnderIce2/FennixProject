#include <task.h>
#include <asm.h>
#include "../cpu/idt.h"
#include <stdarg.h>
#include <syscalls.h>

static uint64_t syscall_unimpl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
    fixme("unimplemented %d %d %d %d %d", a, b, c, d, e);
    return 0;
}

static uint64_t syscall_exit(uint64_t code)
{
    SysGetCurrentThread()->ExitCode = code;
    SysGetCurrentThread()->State = STATE_TERMINATED;
    return 0;
}

static uint64_t syscall_dbg(enum debug_type type, char *message, const char *file, int line, const char *function, va_list args)
{
    debug("syscall_dbg( %d, %s, %s, %d, %s, ... )\n", type, message, file, line, function);
    debug_printf(type, message, file, line, function, args);
    return 0;
}

static void *syscallsTable[] = {
    [_ProcessExit] = syscall_exit,
    [_ProcessCreate] = syscall_unimpl,
    [_ThreadCreate] = syscall_unimpl,
    [_GetCurrentProcess] = syscall_unimpl,
    [_GetCurrentThread] = syscall_unimpl,
    [_Schedule] = syscall_unimpl,

    [_SystemInfo] = syscall_unimpl,
    [_SystemTime] = syscall_unimpl,
    [_SystemTimeSet] = syscall_unimpl,

    [_DebugMessage] = syscall_dbg,
};

static uint64_t syscall_handler(REGISTERS *regs)
{
    fixme("syscall %lld invoked", RAX);
    return 0;
}

static uint64_t protectedmode_syscall_handler(REGISTERS *regs)
{
    err("Not implemented.");
    return 0;
}

void init_syscalls()
{
    // TODO: complete implementation? not sure if this is the way to enable syscallsTable

    // TODO: this should be initialized on every CPU but for now our system is running only on the first CPU
    wrmsr(MSR_STAR, rdmsr(MSR_STAR) | ((uint64_t)0x8 << 32));
    wrmsr(MSR_STAR, rdmsr(MSR_STAR) | ((uint64_t)0x18 << 48));
    wrmsr(MSR_LSTAR, (uint64_t)syscall_handler);
    wrmsr(MSR_SYSCALL_MASK, 0);
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | 1);
    register_interrupt_handler(0xfe, protectedmode_syscall_handler);
    trace("Syscalls has been initialized");
}
