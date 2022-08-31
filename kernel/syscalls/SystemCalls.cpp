#include "syscalls.hpp"

#include "../cpu/gdt.h"
#include "../cpu/idt.h"

#include <internal_task.h>
#include <critical.hpp>
#include <stdarg.h>
#include <task.h>
#include <asm.h>

/*
    If this is going to support multiple operating systems,
    fristly we need to rewrite "SystemCallsHelper.asm" file.

    How syscalls are called: https://github.com/torvalds/linux/blob/master/tools/include/nolibc/arch-x86_64.h#L68
    Syscalls assembly entry: https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_64.S#L87
    Syscalls C entry: https://github.com/torvalds/linux/blob/master/arch/x86/entry/common.c#L73
    And then how it calls the function: https://github.com/torvalds/linux/blob/master/arch/x86/entry/common.c#L40
*/

extern "C" uint64_t syscall_handler(SyscallsRegs *regs)
{
    switch (SysGetCurrentThread()->Info.Platform)
    {
    case Platform::UnknownPlatform:
        static int once = 0;
        if (!once++)
            err("No platform is specified by the process? Guessing Native...");
        [[fallthrough]];
    case Platform::Native:
    {
        return HandleFennixSyscalls(regs);
    }
    case Platform::POSIX: // TODO: add support for POSIX
    case Platform::Linux:
    {
        return HandleLinuxSyscalls(regs);
    }
    case Platform::Windows:
    {
        err("Platform Windows is not supported yet.");
        return -1;
    }
    default:
        return -1;
    }
    return -1;
}

extern "C" void syscall_handle();

void init_syscalls()
{
    // TODO: not tested enough
    // EFER efer;
    // efer.raw = rdmsr(MSR_EFER);
    // efer.SCE = 1;
    // wrmsr(MSR_EFER, efer.raw);

    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | 1);
    wrmsr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE) << 32) | ((uint64_t)(GDT_KERNEL_DATA | 3) << 48));
    wrmsr(MSR_LSTAR, (uint64_t)syscall_handle);
    wrmsr(MSR_SYSCALL_MASK, 0);
    // debug("Syscalls has been initialized");
}
