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
    trace("Userspace thread (%lld) exited with code %lld", SysGetCurrentThread()->ThreadID, code);
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

__attribute__((naked, used)) void syscall_handler_helper()
{
    asm volatile("swapgs\n" // swap the current GS with that one that MSR (0xc0000102) contains
                 "mov %rsp, %gs:(8)\n"
                 "mov %gs:(16), %rsp\n"
                 "sti\n" // enable interrupts
                 "pushq $0x43\n"
                 "pushq %gs:(8)\n"
                 "pushq %r11\n"
                 "pushq $0x3B\n"
                 "pushq %rcx\n"
                 "pushq $0\n"
                 "pushq $0\n"
                 "pushq %rax\n"
                 "pushq %rbx\n"
                 "pushq %rcx\n"
                 "pushq %rdx\n"
                 "pushq %rsi\n"
                 "pushq %rdi\n"
                 "pushq %rbp\n"
                 "pushq %r8\n"
                 "pushq %r9\n"
                 "pushq %r10\n"
                 "pushq %r11\n"
                 "pushq %r12\n"
                 "pushq %r13\n"
                 "pushq %r14\n"
                 "pushq %r15\n"
                 "mov %rsp, %rdi\n"
                 "call syscall_handler\n" // call the handle
                 "popq %r15\n"
                 "popq %r14\n"
                 "popq %r13\n"
                 "popq %r12\n"
                 "popq %r11\n"
                 "popq %r10\n"
                 "popq %r9\n"
                 "popq %r8\n"
                 "popq %rbp\n"
                 "popq %rdi\n"
                 "popq %rsi\n"
                 "popq %rdx\n"
                 "popq %rcx\n"
                 "popq %rbx\n"
                 "popq %rax\n"
                 "addq $56, %rsp\n"
                 "cli\n"    // disable interrupts
                 "swapgs\n" // swap back the GS
                 "sysret\n");
}

static uint64_t syscall_handler(REGISTERS *regs)
{
    if (RAX > sizeof(syscallsTable))
    {
        debug("syscall %d not implemented\n", RAX);
        return -1;
    }
    uint64_t (*call)(unsigned int, ...) = syscallsTable[RAX];
    if (!call)
    {
        err("syscall %#llx failed.", RAX);
        return -2;
    }
    uint64_t ret = call(RBX, RCX, RDX, RSI, RDI);
    RAX = ret;
    return ret;
}

__attribute__((naked, used)) void syscall_interrpt_handler_helper()
{
    asm("cld\n"
        "pushq %rax\n"
        "pushq %rbx\n"
        "pushq %rcx\n"
        "pushq %rdx\n"
        "pushq %rsi\n"
        "pushq %rdi\n"
        "pushq %rbp\n"
        "pushq %r8\n"
        "pushq %r9\n"
        "pushq %r10\n"
        "pushq %r11\n"
        "pushq %r12\n"
        "pushq %r13\n"
        "pushq %r14\n"
        "pushq %r15\n"
        "movq %ds, %rax\n"
        "pushq %rax\n"
        "movw $16, %ax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "movw %ax, %ss\n"
        "movq %rsp, %rdi\n"
        "call syscall_interrpt_handler\n"
        "popq %rax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "popq %r15\n"
        "popq %r14\n"
        "popq %r13\n"
        "popq %r12\n"
        "popq %r11\n"
        "popq %r10\n"
        "popq %r9\n"
        "popq %r8\n"
        "popq %rbp\n"
        "popq %rdi\n"
        "popq %rsi\n"
        "popq %rdx\n"
        "popq %rcx\n"
        "popq %rbx\n"
        "popq %rax\n"
        "addq $16, %rsp\n"
        "iretq");
}

static uint64_t syscall_interrpt_handler(REGISTERS *regs)
{
    if (RAX > sizeof(syscallsTable))
    {
        debug("syscall %d not implemented\n", RAX);
        return -1;
    }
    uint64_t (*call)(unsigned int, ...) = syscallsTable[RAX];
    if (!call)
    {
        err("syscall %#llx failed.", RAX);
        return -2;
    }
    uint64_t ret = call(RBX, RCX, RDX, RSI, RDI);
    RAX = ret;
    return ret;
}

void init_syscalls()
{
    wrmsr(MSR_STAR, rdmsr(MSR_STAR) | ((uint64_t)0x8 << 32));
    wrmsr(MSR_STAR, rdmsr(MSR_STAR) | ((uint64_t)0x18 << 48));
    wrmsr(MSR_LSTAR, (uint64_t)syscall_handler_helper);
    wrmsr(MSR_SYSCALL_MASK, 0);
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | 1);
    trace("Syscalls has been initialized");
}
