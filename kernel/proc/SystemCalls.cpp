#include <task.h>
#include <asm.h>
#include "../cpu/idt.h"
#include "../cpu/gdt.h"
#include "../drivers/serial.h"
#include <stdarg.h>
#include <syscalls.h>
#include <display.h>

static uint64_t internal_unimpl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g)
{
    fixme("Unimplemented Syscall: %d %d %d %d %d %d %d", a, b, c, d, e, f, g);
    return deadcall;
}

static uint64_t internal_exit(uint64_t code)
{
    trace("Userspace thread (%lld) child of %s exited with code %lld", SysGetCurrentThread()->ThreadID, SysGetCurrentThread()->Parent->Name, code);
    SysGetCurrentThread()->ExitCode = code;
    SysGetCurrentThread()->State = STATE_TERMINATED;
    return 0;
}

static uint64_t internal_createprocess(char *Path) { return SysCreateProcessFromFile(Path, true)->ProcessID; }
static uint64_t internal_createthread(uint64_t rip) { return SysCreateThread(SysGetCurrentProcess(), rip, true)->ThreadID; }
static uint64_t internal_getcurrentprocess() { return ((uint64_t)SysGetCurrentProcess()); }
static uint64_t internal_getcurrentthread() { return ((uint64_t)SysGetCurrentThread()); }

static uint64_t internal_fbaddress() { return CurrentDisplay->GetFramebuffer()->Address; }
static uint64_t internal_fbsize() { return CurrentDisplay->GetFramebuffer()->Size; }
static uint64_t internal_fbwidth() { return CurrentDisplay->GetFramebuffer()->Width; }
static uint64_t internal_fbheight() { return CurrentDisplay->GetFramebuffer()->Height; }
static uint64_t internal_fbppsl() { return CurrentDisplay->GetFramebuffer()->PixelsPerScanLine; }

static uint64_t internal_dbg(int port, char *message)
{
    serial_write_text(port, message);
    return 0;
}

static void *syscallsTable[] = {
    // #ifndef __INTELLISENSE__ // hide "array designators are nonstandard in C/C++(2901)" error in vscode
    [_NullCall] = (void *)internal_unimpl,
    [_ProcessExit] = (void *)internal_exit,
    [_ProcessCreate] = (void *)internal_createprocess,
    [_ThreadCreate] = (void *)internal_createthread,
    [_GetCurrentProcess] = (void *)internal_getcurrentprocess,
    [_GetCurrentThread] = (void *)internal_getcurrentthread,
    [_Schedule] = (void *)internal_unimpl,

    [_SystemInfo] = (void *)internal_unimpl,
    [_SystemTime] = (void *)internal_unimpl,
    [_SystemTimeSet] = (void *)internal_unimpl,

    [_GetFramebufferAddress] = (void *)internal_fbaddress,
    [_GetFramebufferSize] = (void *)internal_fbsize,
    [_GetFramebufferWidth] = (void *)internal_fbwidth,
    [_GetFramebufferHeight] = (void *)internal_fbheight,
    [_GetFramebufferPixelsPerScanLine] = (void *)internal_fbppsl,

    [_RegisterInterruptHandler] = (void *)internal_unimpl,
    [_UnregisterInterruptHandler] = (void *)internal_unimpl,

    [_DebugMessage] = (void *)internal_dbg,
    // #endif
};

// TODO: fully port the syscall handler to inline assembly
__attribute__((naked, used, aligned(0x1000))) void syscall_handler_helper()
{
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
}

typedef struct _SyscallsRegs
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_num, error_code, rip, cs, rflags, rsp, ss;
} SyscallsRegs;

extern "C" uint64_t syscall_handler(SyscallsRegs *regs)
{
    if (RAX > sizeof(syscallsTable))
    {
        return internal_unimpl(regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi, regs->rbp);
    }
    uint64_t (*call)(uint64_t, ...) = reinterpret_cast<uint64_t (*)(uint64_t, ...)>(syscallsTable[RAX]);
    if (!call)
    {
        err("syscall %#llx failed.", RAX);
        return failedcall;
    }
    // TODO: i should add a fifth argument to the syscall
    uint64_t ret = call(RBX, RDX, RSI, RDI, 0);
    RAX = ret;
    return ret;
}

extern "C"
{
    __attribute__((naked, used)) void syscall_interrpt_handler_helper()
    {
    }

    static uint64_t syscall_interrpt_handler(REGISTERS *regs)
    {
        return 0;
    }
}

extern "C" void syscall_handle();

void init_syscalls()
{
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | 1);
    wrmsr(MSR_STAR, ((uint64_t)(GDT_KERNEL_CODE) << 32) | ((uint64_t)(GDT_KERNEL_DATA | 3) << 48));
    wrmsr(MSR_LSTAR, (uint64_t)syscall_handle);
    wrmsr(MSR_SYSCALL_MASK, 0);
    trace("Syscalls has been initialized");
}
