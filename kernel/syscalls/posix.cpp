#include "syscalls.hpp"

#include "../../libc/include/syscalls.h"

static uint64_t internal_unimpl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g)
{
    fixme("Unimplemented Syscall: %d %d %d %d %d %d %d", a, b, c, d, e, f, g);
    return deadcall;
}

// TODO: add POSIX support. Syscalls list: https://filippo.io/linux-syscall-table/

static void *POSIXSyscallsTable[] = {
    [0] = (void *)internal_unimpl,
};

uint64_t HandlePOSIXSyscalls(SyscallsRegs *regs)
{
    if (RAX > sizeof(POSIXSyscallsTable))
    {
        return internal_unimpl(regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi, regs->rbp);
    }

    uint64_t (*call)(uint64_t, ...) = reinterpret_cast<uint64_t (*)(uint64_t, ...)>(POSIXSyscallsTable[RAX]);
    if (!call)
    {
        err("Syscall %#llx failed.", RAX);
        return failedcall;
    }
    uint64_t ret = call(RBX, RDX, RSI, RDI, regs);
    RAX = ret;
    return ret;
}
