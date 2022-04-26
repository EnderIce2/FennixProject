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

#ifdef __x86_64__
__attribute__((naked)) static inline unsigned long syscall(enum SystemCalls call, ...)
{
    asm volatile(
        "push %rbp \n\t"
        "mov %rsp, %rbp \n\t"
        "mov %rdi, %rax \n\t"
        "mov %rsi, %rdi \n\t"
        "mov %rdx, %rsi \n\t"
        "mov %rcx, %rdx \n\t"
        "mov %r8, %r10 \n\t"
        "mov %r9, %r8 \n\t"
        "mov 8(%rsp), %r9 \n\t"
        "syscall \n\t"
        "mov %rbp, %rsp \n\t"
        "pop %rbp \n\t"
        "ret");
}
#else
__attribute__((naked)) static inline unsigned long syscall(enum SystemCalls call, ...)
{
    asm volatile(
        "push %rbp \n\t"
        "mov %rsp, %rbp \n\t"
        "mov %rdi, %rax \n\t"
        "mov %rsi, %rdi \n\t"
        "mov %rdx, %rsi \n\t"
        "mov %rcx, %rdx \n\t"
        "mov %r8, %r10 \n\t"
        "mov %r9, %r8 \n\t"
        "mov 8(%rsp), %r9 \n\t"
        "int $0xfe \n\t"
        "mov %rbp, %rsp \n\t"
        "pop %rbp \n\t"
        "ret");
}
#endif
