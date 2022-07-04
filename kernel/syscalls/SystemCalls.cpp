#include "syscalls.hpp"

#include "../cpu/gdt.h"
#include "../cpu/idt.h"

#include <internal_task.h>
#include <critical.hpp>
#include <stdarg.h>
#include <task.h>
#include <asm.h>

// TODO: fully port the syscall handler to inline assembly
__attribute__((naked, used, aligned(0x1000))) void syscall_handler_helper()
{
    EnterCriticalSection;
    asm("swapgs\n");
    asm("movq %rsp, %gs:0x8\n");
    // asm("movq %gs:0x0, %rsp\n");

    asm("pushq $0x1b\n");
    asm("pushq %gs:0x8\n");
    asm("pushq %r11\n");
    asm("pushq $0x23\n");
    asm("pushq %rcx\n");
    asm("cld\n");

    asm("pushq %rax\n");
    asm("pushq %rbx\n");
    asm("pushq %rcx\n");
    asm("pushq %rdx\n");
    asm("pushq %rsi\n");
    asm("pushq %rdi\n");
    asm("pushq %rbp\n");
    asm("pushq %r8\n");
    asm("pushq %r9\n");
    asm("pushq %r10\n");
    asm("pushq %r11\n");
    asm("pushq %r12\n");
    asm("pushq %r13\n");
    asm("pushq %r14\n");
    asm("pushq %r15\n");

    asm("movq %rsp, %rdi\n");
    asm("movq $0, %rbp\n");
    asm("call syscall_handler\n");

    asm("popq %r15\n");
    asm("popq %r14\n");
    asm("popq %r13\n");
    asm("popq %r12\n");
    asm("popq %r11\n");
    asm("popq %r10\n");
    asm("popq %r9\n");
    asm("popq %r8\n");
    asm("popq %rbp\n");
    asm("popq %rdi\n");
    asm("popq %rsi\n");
    asm("popq %rdx\n");
    asm("popq %rcx\n");
    asm("popq %rbx\n");

    asm("mov %gs:0x8, %rsp\n");
    asm("swapgs\n");
    asm("sysretq\n");
    LeaveCriticalSection;
}

extern "C" uint64_t syscall_handler(SyscallsRegs *regs)
{
    switch (SysGetCurrentThread()->Info.Platform)
    {
    case Platform::UnknownPlatform:
        err("No platform is specified by the process? Guessing Native...");
        [[fallthrough]];
    case Platform::Native:
    {
        return HandleFennixSyscalls(regs);
    }
    case Platform::POSIX:
    {
        return HandlePOSIXSyscalls(regs);
    }
    case Platform::Linux:
    {
        err("Platform Linux is not supported yet.");
        return -1;
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
