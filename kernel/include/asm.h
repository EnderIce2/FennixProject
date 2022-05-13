#pragma once

#include <types.h>
#include <interrupts.h>
#include <cputables.h>
#include "cpuid.h"

// Assembly inline functions.

static inline void IOWait()
{
    asm volatile("outb %%al, $0x80"
                 :
                 : "a"(0));
}

static inline void lgdt(GlobalDescriptorTableDescriptor gdt)
{
    asm volatile("lgdt %0"
                 :
                 : "m"(gdt));
}

static inline void lidt(InterruptDescriptorTableDescriptor idt)
{
    asm("lidt %0"
        :
        : "m"(idt));
}

static inline void ltr(uint16_t segment)
{
    asm volatile("ltr %%ax"
                 :
                 : "a"(segment));
}

static inline void invlpg(uint64_t address)
{
    asm volatile("invlpg (%0);" ::"r"(address)
                 : "memory");
}

static inline void cpuid(uint32_t val, uint32_t *rax, uint32_t *rbx, uint32_t *rcx, uint32_t *rdx)
{
    int arr[4] = {0};
    asm("cpuid"
        : "=a"(arr[0]), "=b"(arr[1]), "=c"(arr[2]), "=d"(arr[3])
        : "a"(val), "c"(0));
    if (rax)
    {
        *rax = arr[0];
    }
    if (rbx)
    {
        *rbx = arr[1];
    }
    if (rcx)
    {
        *rcx = arr[2];
    }
    if (rdx)
    {
        *rdx = arr[3];
    }
}

static inline int GetCPUIDMax(uint32_t leaf, uint32_t *signature)
{
    // TODO: in 32bit is different?
    uint32_t rax, rbx, rcx, rdx;
    cpuid(leaf, &rax, &rbx, &rcx, &rdx);
    if (signature)
        *signature = rbx;
    return rax;
}

static inline int GetCPUID(uint32_t leaf, uint32_t *rax, uint32_t *rbx, uint32_t *rcx, uint32_t *rdx)
{
    uint32_t max_leaf = GetCPUIDMax(leaf & 0x80000000, 0);
    if (max_leaf == 0 || max_leaf < leaf)
        return 0;
    cpuid(leaf, rax, rbx, rcx, rdx);
    return 1;
}

static inline bool InterruptsEnabled()
{
    RFLAGS flags;
    asm volatile(
        "pushfq\n"
        "pop %0"
        : "=r"(flags.raw));
    return flags.IF == 1;
}

#define CLI asm volatile("cli" :: \
                             : "memory")
#define STI asm volatile("sti" :: \
                             : "memory")

#define HLT asm volatile("hlt")

#define PAUSE asm volatile("pause")

static inline void mem_barrier()
{
    asm volatile("" ::
                     : "memory");
}

static inline void mem_fence()
{
    asm volatile("mfence" ::
                     : "memory");
}

static inline void store_fence()
{
    asm volatile("sfence" ::
                     : "memory");
}

static inline void load_fence()
{
    asm volatile("lfence" ::
                     : "memory");
}

// TODO: Does this actually work?
static inline void ENABLE_NX()
{
    asm volatile(
        "movl $0xc0000080, %%ecx\n\t"
        "rdmsr\n\t"
        "btsl $11, %%eax\n\t"
        "wrmsr\n\t" ::
            : "eax", "ecx", "edx", "memory");
}

enum MSRID
{
    MSR_APIC = 0x1B,
    MSR_CR_PAT = 0x00000277,

    MSR_CR_PAT_RESET = 0x0007040600070406ULL,
    /** @brief Extended Feature Enable Register (0xc0000080) */
    MSR_EFER = 0xC0000080,
    /** @brief legacy SYSCALL (0xC0000081) */
    MSR_STAR = 0xC0000081,
    /** @brief 64bit SYSCALL (0xC0000082) */
    MSR_LSTAR = 0xC0000082,
    /** @brief compatibility mode SYSCALL (0xC0000083) */
    MSR_CSTAR = 0xC0000083,
    /** @brief EFLAGS mask for syscall (0xC0000084) */
    MSR_SYSCALL_MASK = 0xC0000084,
    /** @brief 64bit FS base (0xC0000100) */
    MSR_FS_BASE = 0xC0000100,
    /** @brief 64bit GS base (0xC0000101) */
    MSR_GS_BASE = 0xC0000101,
    /** @brief SwapGS GS shadow (0xC0000102) */
    MSR_SHADOW_GS_BASE = 0xC0000102,
    /** @brief Auxiliary TSC (0xC0000103) */
    MSR_TSC_AUX = 0xC0000103

};

static inline uint64_t rdmsr(enum MSRID msr)
{
    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"(msr));
    return ((uint64_t)low) | (((uint64_t)high) << 32);
}

static inline void wrmsr(enum MSRID msr, uint64_t Value)
{
    uint32_t val1 = Value, val2 = Value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"(msr), "a"(val1), "d"(val2));
}

static inline CR0 readcr0()
{
    uint64_t Result;
    asm volatile("mov %%cr0, %[Result]"
                 : [Result] "=q"(Result));
    return (CR0){.raw = Result};
}

static inline CR2 readcr2()
{
    uint64_t Result;
    asm volatile("mov %%cr2, %[Result]"
                 : [Result] "=q"(Result));
    return (CR2){.raw = Result};
}

static inline CR3 readcr3()
{
    uint64_t Result;
    asm volatile("mov %%cr3, %[Result]"
                 : [Result] "=q"(Result));
    return (CR3){.raw = Result};
}

static inline CR4 readcr4()
{
    uint64_t Result;
    asm volatile("mov %%cr4, %[Result]"
                 : [Result] "=q"(Result));
    return (CR4){.raw = Result};
}

static inline CR8 readcr8()
{
    uint64_t Result;
    asm volatile("mov %%cr8, %[Result]"
                 : [Result] "=q"(Result));
    return (CR8){.raw = Result};
}

static inline void writecr0(CR0 ControlRegister)
{
    asm volatile("mov %[ControlRegister], %%cr0"
                 :
                 : [ControlRegister] "q"(ControlRegister.raw)
                 : "memory");
}

static inline void writecr2(CR2 ControlRegister)
{
    asm volatile("mov %[ControlRegister], %%cr2"
                 :
                 : [ControlRegister] "q"(ControlRegister.raw)
                 : "memory");
}

static inline void writecr3(CR3 ControlRegister)
{
    asm volatile("mov %[ControlRegister], %%cr3"
                 :
                 : [ControlRegister] "q"(ControlRegister.raw)
                 : "memory");
}

static inline void writecr4(CR4 ControlRegister)
{
    asm volatile("mov %[ControlRegister], %%cr4"
                 :
                 : [ControlRegister] "q"(ControlRegister.raw)
                 : "memory");
}

static inline void writecr8(CR8 ControlRegister)
{
    asm volatile("mov %[ControlRegister], %%cr8"
                 :
                 : [ControlRegister] "q"(ControlRegister.raw)
                 : "memory");
}

static inline void mwait(size_t rax, size_t rcx)
{
    asm volatile("mwait"
                 :
                 : "a"(rax), "c"(rcx));
}

static inline void monitor(size_t rax, size_t rcx, size_t rdx)
{
    asm volatile("monitor"
                 :
                 : "a"(rax), "c"(rcx), "d"(rdx));
}
