#include <task.h>
#include <asm.h>
#include <stdarg.h>

static uint32_t syscall_dbg(uint32_t code, enum debug_type type, char *message, const char *file, int line, const char *function, va_list args)
{
    debug("syscall_dbg( %d, %d, %s, %s, %d, %s, ... )\n", code, type, message, file, line, function);
    debug_printf(type, message, file, line, function, args);
    return 0;
}

static void *syscalls[] = {

    [_DebugMessage] = syscall_dbg,
};

extern void syscall_handler_helper();

uint32_t syscalls_handler(REGISTERS *regs)
{
    trace("syscall invoked");
    TRACEREGS(regs);
    int rdx = RAX;
    if (rdx >= sizeof(syscalls))
    {
        debug("syscall %d not implemented\n", rdx);
        return 2;
    }
    uint32_t (*func)(unsigned int, ...) = syscalls[rdx];
    if (!func)
        return 1;
    // memcpy(&current_thread->regs, regs, sizeof(REGISTERS));
    uint32_t ret = func(RBX, RCX, RDX, RSI, RDI);
    RAX = ret;
    return 0;
}

void init_syscalls()
{
    // TODO: complete implementation? not sure if this is the way to enable syscalls

    // TODO: this should be initialized on every CPU but for now our system is running only on the first CPU
    wrmsr(0xC0000081, rdmsr(0xC0000081) | ((uint64_t)0x8 << 32));
    wrmsr(0xC0000081, rdmsr(0xC0000081) | ((uint64_t)0x18 << 48));
    wrmsr(0xC0000082, (uint64_t)syscall_handler_helper);
    wrmsr(0xC0000084, 0);
    wrmsr(0xC0000080, rdmsr(0xC0000080) | 1);
    trace("Syscalls has been initialized");
}
