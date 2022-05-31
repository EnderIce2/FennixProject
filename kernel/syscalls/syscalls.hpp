#pragma once

#include <stdint.h>

// #define DEBUG_SYSCALLS 1

#ifdef DEBUG_SYSCALLS
#define syscldbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define syscldbg(m, ...)
#endif

typedef struct _SyscallsRegs
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_num, error_code, rip, cs, rflags, rsp, ss;
} SyscallsRegs;

uint64_t HandleFennixSyscalls(SyscallsRegs *regs);
uint64_t HandlePOSIXSyscalls(SyscallsRegs *regs);
