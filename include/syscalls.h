#pragma once

enum SystemCalls
{
    _ProcessExit = 1,
    _ProcessCreate,
    _ThreadCreate,
    _GetCurrentProcess,
    _GetCurrentThread,
    _Schedule,

    _SystemInfo,
    _SystemTime,
    _SystemTimeSet,

    _DebugMessage,
};

#ifndef asm
#define asm __asm__
#endif

// #define INTERRUPT_SYSCALL 1

__attribute__((naked)) static inline unsigned long syscall(enum SystemCalls call, ...)
{
#ifndef INTERRUPT_SYSCALL
    asm volatile("push %rbp\n"
                 "mov %rsp, %rbp\n"
                 "mov %rdi, %rax\n"
                 "mov %rsi, %rdi\n"
                 "mov %rdx, %rsi\n"
                 "mov %rcx, %rdx\n"
                 "mov %r8, %r10\n"
                 "mov %r9, %r8\n"
                 "mov 8(%rsp), %r9\n"
                 "syscall\n"
                 "mov %rbp, %rsp\n"
                 "pop %rbp\n"
                 "ret");
#else
    asm volatile("push %rbp\n"
                 "mov %rsp, %rbp\n"
                 "mov %rdi, %rax\n"
                 "mov %rsi, %rdi\n"
                 "mov %rdx, %rsi\n"
                 "mov %rcx, %rdx\n"
                 "mov %r8, %r10\n"
                 "mov %r9, %r8\n"
                 "mov 8(%rsp), %r9\n"
                 "int $0xfe\n"
                 "mov %rbp, %rsp\n"
                 "pop %rbp\n"
                 "ret");
#endif
}
