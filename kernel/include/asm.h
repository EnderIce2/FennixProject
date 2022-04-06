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

/* Function with 2 arguments */
static inline void cpuid_2(int code, uint32_t *eax, uint32_t *edx)
{
    asm volatile("cpuid"
                 : "=a"(*eax), "=d"(*edx)
                 : "a"(code)
                 : "ecx", "ebx");
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

static inline bool interrupts_enabled()
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

// static inline uint64_t rdmsr(uint64_t msr)
// {
// 	uint32_t low, high;
// 	asm volatile (
// 		"rdmsr"
// 		: "=a"(low), "=d"(high)
// 		: "c"(msr)
// 	);
// 	return ((uint64_t)high << 32) | low;
// }

static inline uint64_t rdmsr(uint32_t msr)
{
    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"(msr));
    return ((uint64_t)low) | (((uint64_t)high) << 32);
}

// static inline void wrmsr(uint32_t msr_id, uint64_t msr_value)
// {
//     asm volatile ( "wrmsr" : : "c" (msr_id), "A" (msr_value) );
// }

static inline void wrmsr(uint32_t msr, uint64_t Value)
{
    uint32_t val1 = Value, val2 = Value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"(msr), "a"(val1), "d"(val2));
}

static inline CR0 readcr0()
{
    uint64_t res;
    asm volatile("mov %%cr0, %[res]"
                 : [res] "=q"(res));
    return (CR0){.raw = res};
}

static inline uint64_t readcr2()
{
    uint64_t res;
    asm volatile("mov %%cr2, %[res]"
                 : [res] "=q"(res));
    return res;
}

static inline uint64_t readcr3()
{
    uint64_t res;
    asm volatile("mov %%cr3, %[res]"
                 : [res] "=q"(res));
    return res;
}

static inline CR4 readcr4()
{
    uint64_t res;
    asm volatile("mov %%cr4, %[res]"
                 : [res] "=q"(res));
    return (CR4){.raw = res};
}

static inline uint64_t readcr8()
{
    uint64_t res;
    asm volatile("mov %%cr8, %[res]"
                 : [res] "=q"(res));
    return res;
}

static inline void writecr0(CR0 base)
{
    asm volatile("mov %[base], %%cr0"
                 :
                 : [base] "q"(base.raw)
                 : "memory");
}

static inline void writecr2(uint64_t base)
{
    asm volatile("mov %[base], %%cr2"
                 :
                 : [base] "q"(base)
                 : "memory");
}

static inline void writecr3(uint64_t base)
{
    asm volatile("mov %[base], %%cr3"
                 :
                 : [base] "q"(base)
                 : "memory");
}

static inline void writecr4(CR4 base)
{
    asm volatile("mov %[base], %%cr4"
                 :
                 : [base] "q"(base.raw)
                 : "memory");
}

static inline void writecr8(uint64_t base)
{
    asm volatile("mov %[base], %%cr8"
                 :
                 : [base] "q"(base)
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
