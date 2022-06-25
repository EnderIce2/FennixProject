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

// https://github.com/pp3345/sysfs-msrs/blob/master/msrs.h

enum MSRID
{
    MSR_MONITOR_FILTER_SIZE = 0x6,
    MSR_TIME_STAMP_COUNTER = 0x10,
    MSR_PLATFORM_ID = 0x17,
    MSR_APIC_BASE = 0x1B,
    MSR_FEATURE_CONTROL = 0x3A,
    MSR_TSC_ADJUST = 0x3B,
    MSR_SPEC_CTRL = 0x48,
    MSR_PRED_CMD = 0x49,
    MSR_BIOS_UPDT_TRIG = 0x79,
    MSR_BIOS_SIGN_ID = 0x8B,
    MSR_SGXLEPUBKEYHASH0 = 0x8C,
    MSR_SGXLEPUBKEYHASH1 = 0x8D,
    MSR_SGXLEPUBKEYHASH2 = 0x8E,
    MSR_SGXLEPUBKEYHASH3 = 0x8F,
    MSR_SMM_MONITOR_CTL = 0x9B,
    MSR_SMBASE = 0x9E,
    MSR_PMC0 = 0xC1,
    MSR_PMC1 = 0xC2,
    MSR_PMC2 = 0xC3,
    MSR_PMC3 = 0xC4,
    MSR_PMC4 = 0xC5,
    MSR_PMC5 = 0xC6,
    MSR_PMC6 = 0xC7,
    MSR_PMC7 = 0xC8,
    MSR_UMWAIT_CONTROL = 0xE1,
    MSR_MPERF = 0xE7,
    MSR_APERF = 0xE8,
    MSR_MTRRCAP = 0xFE,
    MSR_ARCH_CAPABILITIES = 0x10A,
    MSR_FLUSH_CMD = 0x10B,
    MSR_SYSENTER_CS = 0x17A,
    MSR_SYSENTER_ESP = 0x175,
    MSR_SYSENTER_EIP = 0x176,
    MSR_MCG_CAP = 0x179,
    MSR_MCG_STATUS = 0x17A,
    MSR_MCG_CTL = 0x17B,
    MSR_PERFEVTSEL0 = 0x186,
    MSR_PERFEVTSEL1 = 0x187,
    MSR_PERFEVTSEL2 = 0x188,
    MSR_PERFEVTSEL3 = 0x189,
    MSR_PERF_STATUS = 0x198,
    MSR_PERF_CTL = 0x199,
    MSR_CLOCK_MODULATION = 0x19A,
    MSR_THERM_INTERRUPT = 0x19B,
    MSR_THERM_STATUS = 0x19C,
    MSR_MISC_ENABLE = 0x1A0,
    MSR_ENERGY_PERF_BIAS = 0x1B0,
    MSR_PACKAGE_THERM_STATUS = 0x1B1,
    MSR_PACKAGE_THERM_INTERRUPT = 0x1B2,
    MSR_DEBUGCTL = 0x1D9,
    MSR_SMRR_PHYSBASE = 0x1F2,
    MSR_SMRR_PHYSMASK = 0x1F3,
    MSR_PLATFORM_DCA_CAP = 0x1F8,
    MSR_CPU_DCA_CAP = 0x1F9,
    MSR_DCA_0_CAP = 0x1FA,
    MSR_MTRR_PHYSBASE0 = 0x200,
    MSR_MTRR_PHYSMASK0 = 0x201,
    MSR_MTRR_PHYSBASE1 = 0x202,
    MSR_MTRR_PHYSMASK1 = 0x203,
    MSR_MTRR_PHYSBASE2 = 0x204,
    MSR_MTRR_PHYSMASK2 = 0x205,
    MSR_MTRR_PHYSBASE3 = 0x206,
    MSR_MTRR_PHYSMASK3 = 0x207,
    MSR_MTRR_PHYSBASE4 = 0x208,
    MSR_MTRR_PHYSMASK4 = 0x209,
    MSR_MTRR_PHYSBASE5 = 0x20A,
    MSR_MTRR_PHYSMASK5 = 0x20B,
    MSR_MTRR_PHYSBASE6 = 0x20C,
    MSR_MTRR_PHYSMASK6 = 0x20D,
    MSR_MTRR_PHYSBASE7 = 0x20E,
    MSR_MTRR_PHYSMASK7 = 0x20F,
    MSR_MTRR_PHYSBASE8 = 0x210,
    MSR_MTRR_PHYSMASK8 = 0x211,
    MSR_MTRR_PHYSBASE9 = 0x212,
    MSR_MTRR_PHYSMASK9 = 0x213,
    MSR_MTRR_FIX64K_00000 = 0x250,
    MSR_MTRR_FIX16K_80000 = 0x258,
    MSR_MTRR_FIX16K_A0000 = 0x259,
    MSR_MTRR_FIX4K_C0000 = 0x268,
    MSR_MTRR_FIX4K_C8000 = 0x269,
    MSR_MTRR_FIX4K_D0000 = 0x26A,
    MSR_MTRR_FIX4K_D8000 = 0x26B,
    MSR_MTRR_FIX4K_E0000 = 0x26C,
    MSR_MTRR_FIX4K_E8000 = 0x26D,
    MSR_MTRR_FIX4K_F0000 = 0x26E,
    MSR_MTRR_FIX4K_F8000 = 0x26F,
    MSR_PAT = 0x277,
    MSR_MC0_CTL2 = 0x280,
    MSR_MC1_CTL2 = 0x281,
    MSR_MC2_CTL2 = 0x282,
    MSR_MC3_CTL2 = 0x283,
    MSR_MC4_CTL2 = 0x284,
    MSR_MC5_CTL2 = 0x285,
    MSR_MC6_CTL2 = 0x286,
    MSR_MC7_CTL2 = 0x287,
    MSR_MC8_CTL2 = 0x288,
    MSR_MC9_CTL2 = 0x289,
    MSR_MC10_CTL2 = 0x28A,
    MSR_MC11_CTL2 = 0x28B,
    MSR_MC12_CTL2 = 0x28C,
    MSR_MC13_CTL2 = 0x28D,
    MSR_MC14_CTL2 = 0x28E,
    MSR_MC15_CTL2 = 0x28F,
    MSR_MC16_CTL2 = 0x290,
    MSR_MC17_CTL2 = 0x291,
    MSR_MC18_CTL2 = 0x292,
    MSR_MC19_CTL2 = 0x293,
    MSR_MC20_CTL2 = 0x294,
    MSR_MC21_CTL2 = 0x295,
    MSR_MC22_CTL2 = 0x296,
    MSR_MC23_CTL2 = 0x297,
    MSR_MC24_CTL2 = 0x298,
    MSR_MC25_CTL2 = 0x299,
    MSR_MC26_CTL2 = 0x29A,
    MSR_MC27_CTL2 = 0x29B,
    MSR_MC28_CTL2 = 0x29C,
    MSR_MC29_CTL2 = 0x29D,
    MSR_MC30_CTL2 = 0x29E,
    MSR_MC31_CTL2 = 0x29F,
    MSR_MTRR_DEF_TYPE = 0x2FF,
    MSR_FIXED_CTR0 = 0x309,
    MSR_FIXED_CTR1 = 0x30A,
    MSR_FIXED_CTR2 = 0x30B,
    MSR_PERF_CAPABILITIES = 0x345,
    MSR_FIXED_CTR_CTRL = 0x38D,
    MSR_PERF_GLOBAL_STATUS = 0x38E,
    MSR_PERF_GLOBAL_CTRL = 0x38F,
    MSR_PERF_GLOBAL_STATUS_RESET = 0x390,
    MSR_PERF_GLOBAL_STATUS_SET = 0x391,
    MSR_PERF_GLOBAL_INUSE = 0x392,
    MSR_PEBS_ENABLE = 0x3F1,
    MSR_MC0_CTL = 0x400,
    MSR_MC0_STATUS = 0x401,
    MSR_MC0_ADDR = 0x402,
    MSR_MC0_MISC = 0x403,
    MSR_MC1_CTL = 0x404,
    MSR_MC1_STATUS = 0x405,
    MSR_MC1_ADDR = 0x406,
    MSR_MC1_MISC = 0x407,
    MSR_MC2_CTL = 0x408,
    MSR_MC2_STATUS = 0x409,
    MSR_MC2_ADDR = 0x40A,
    MSR_MC2_MISC = 0x40B,
    MSR_MC3_CTL = 0x40C,
    MSR_MC3_STATUS = 0x40D,
    MSR_MC3_ADDR = 0x40E,
    MSR_MC3_MISC = 0x40F,
    MSR_MC4_CTL = 0x410,
    MSR_MC4_STATUS = 0x411,
    MSR_MC4_ADDR = 0x412,
    MSR_MC4_MISC = 0x413,
    MSR_MC5_CTL = 0x414,
    MSR_MC5_STATUS = 0x415,
    MSR_MC5_ADDR = 0x416,
    MSR_MC5_MISC = 0x417,
    MSR_MC6_CTL = 0x418,
    MSR_MC6_STATUS = 0x419,
    MSR_MC6_ADDR = 0x41A,
    MSR_MC6_MISC = 0x41B,
    MSR_MC7_CTL = 0x41C,
    MSR_MC7_STATUS = 0x41D,
    MSR_MC7_ADDR = 0x41E,
    MSR_MC7_MISC = 0x41F,
    MSR_MC8_CTL = 0x420,
    MSR_MC8_STATUS = 0x421,
    MSR_MC8_ADDR = 0x422,
    MSR_MC8_MISC = 0x423,
    MSR_MC9_CTL = 0x424,
    MSR_MC9_STATUS = 0x425,
    MSR_MC9_ADDR = 0x426,
    MSR_MC9_MISC = 0x427,
    MSR_MC10_CTL = 0x428,
    MSR_MC10_STATUS = 0x429,
    MSR_MC10_ADDR = 0x42A,
    MSR_MC10_MISC = 0x42B,
    MSR_MC11_CTL = 0x42C,
    MSR_MC11_STATUS = 0x42D,
    MSR_MC11_ADDR = 0x42E,
    MSR_MC11_MISC = 0x42F,
    MSR_MC12_CTL = 0x430,
    MSR_MC12_STATUS = 0x431,
    MSR_MC12_ADDR = 0x432,
    MSR_MC12_MISC = 0x433,
    MSR_MC13_CTL = 0x434,
    MSR_MC13_STATUS = 0x435,
    MSR_MC13_ADDR = 0x436,
    MSR_MC13_MISC = 0x437,
    MSR_MC14_CTL = 0x438,
    MSR_MC14_STATUS = 0x439,
    MSR_MC14_ADDR = 0x43A,
    MSR_MC14_MISC = 0x43B,
    MSR_MC15_CTL = 0x43C,
    MSR_MC15_STATUS = 0x43D,
    MSR_MC15_ADDR = 0x43E,
    MSR_MC15_MISC = 0x43F,
    MSR_MC16_CTL = 0x440,
    MSR_MC16_STATUS = 0x441,
    MSR_MC16_ADDR = 0x442,
    MSR_MC16_MISC = 0x443,
    MSR_MC17_CTL = 0x444,
    MSR_MC17_STATUS = 0x445,
    MSR_MC17_ADDR = 0x446,
    MSR_MC17_MISC = 0x447,
    MSR_MC18_CTL = 0x448,
    MSR_MC18_STATUS = 0x449,
    MSR_MC18_ADDR = 0x44A,
    MSR_MC18_MISC = 0x44B,
    MSR_MC19_CTL = 0x44C,
    MSR_MC19_STATUS = 0x44D,
    MSR_MC19_ADDR = 0x44E,
    MSR_MC19_MISC = 0x44F,
    MSR_MC20_CTL = 0x450,
    MSR_MC20_STATUS = 0x451,
    MSR_MC20_ADDR = 0x452,
    MSR_MC20_MISC = 0x453,
    MSR_MC21_CTL = 0x454,
    MSR_MC21_STATUS = 0x455,
    MSR_MC21_ADDR = 0x456,
    MSR_MC21_MISC = 0x457,
    MSR_MC22_CTL = 0x458,
    MSR_MC22_STATUS = 0x459,
    MSR_MC22_ADDR = 0x45A,
    MSR_MC22_MISC = 0x45B,
    MSR_MC23_CTL = 0x45C,
    MSR_MC23_STATUS = 0x45D,
    MSR_MC23_ADDR = 0x45E,
    MSR_MC23_MISC = 0x45F,
    MSR_MC24_CTL = 0x460,
    MSR_MC24_STATUS = 0x461,
    MSR_MC24_ADDR = 0x462,
    MSR_MC24_MISC = 0x463,
    MSR_MC25_CTL = 0x464,
    MSR_MC25_STATUS = 0x465,
    MSR_MC25_ADDR = 0x466,
    MSR_MC25_MISC = 0x467,
    MSR_MC26_CTL = 0x468,
    MSR_MC26_STATUS = 0x469,
    MSR_MC26_ADDR = 0x46A,
    MSR_MC26_MISC = 0x46B,
    MSR_MC27_CTL = 0x46C,
    MSR_MC27_STATUS = 0x46D,
    MSR_MC27_ADDR = 0x46E,
    MSR_MC27_MISC = 0x46F,
    MSR_MC28_CTL = 0x470,
    MSR_MC28_STATUS = 0x471,
    MSR_MC28_ADDR = 0x472,
    MSR_MC28_MISC = 0x473,
    MSR_VMX_BASIC = 0x480,
    MSR_VMX_PINBASED_CTLS = 0x481,
    MSR_VMX_PROCBASED_CTLS = 0x482,
    MSR_VMX_EXIT_CTLS = 0x483,
    MSR_VMX_ENTRY_CTLS = 0x484,
    MSR_VMX_MISC = 0x485,
    MSR_VMX_CR0_FIXED0 = 0x486,
    MSR_VMX_CR0_FIXED1 = 0x487,
    MSR_VMX_CR4_FIXED0 = 0x488,
    MSR_VMX_CR4_FIXED1 = 0x489,
    MSR_VMX_VMCS_ENUM = 0x48A,
    MSR_VMX_PROCBASED_CTLS2 = 0x48B,
    MSR_VMX_EPT_VPID_CAP = 0x48C,
    MSR_VMX_TRUE_PINBASED_CTLS = 0x48D,
    MSR_VMX_TRUE_PROCBASED_CTLS = 0x48E,
    MSR_VMX_TRUE_EXIT_CTLS = 0x48F,
    MSR_VMX_TRUE_ENTRY_CTLS = 0x490,
    MSR_VMX_VMFUNC = 0x491,
    MSR_A_PMC0 = 0x4C1,
    MSR_A_PMC1 = 0x4C2,
    MSR_A_PMC2 = 0x4C3,
    MSR_A_PMC3 = 0x4C4,
    MSR_A_PMC4 = 0x4C5,
    MSR_A_PMC5 = 0x4C6,
    MSR_A_PMC6 = 0x4C7,
    MSR_A_PMC7 = 0x4C8,
    MSR_MCG_EXT_CTL = 0x4D0,
    MSR_SGX_SVN_STATUS = 0x500,
    MSR_RTIT_OUTPUT_BASE = 0x560,
    MSR_RTIT_OUTPUT_MASK_PTRS = 0x561,
    MSR_RTIT_CTL = 0x570,
    MSR_RTIT_STATUS = 0x571,
    MSR_RTIT_CR3_MATCH = 0x572,
    MSR_RTIT_ADDR0_A = 0x580,
    MSR_RTIT_ADDR0_B = 0x581,
    MSR_RTIT_ADDR1_A = 0x582,
    MSR_RTIT_ADDR1_B = 0x583,
    MSR_RTIT_ADDR2_A = 0x584,
    MSR_RTIT_ADDR2_B = 0x585,
    MSR_RTIT_ADDR3_A = 0x586,
    MSR_RTIT_ADDR3_B = 0x587,
    MSR_DS_AREA = 0x600,
    MSR_TSC_DEADLINE = 0x6E0,
    MSR_PM_ENABLE = 0x770,
    MSR_HWP_CAPABILITIES = 0x771,
    MSR_HWP_REQUEST_PKG = 0x772,
    MSR_HWP_INTERRUPT = 0x773,
    MSR_HWP_REQUEST = 0x774,
    MSR_HWP_STATUS = 0x777,
    MSR_X2APIC_APICID = 0x802,
    MSR_X2APIC_VERSION = 0x803,
    MSR_X2APIC_TPR = 0x808,
    MSR_X2APIC_PPR = 0x80A,
    MSR_X2APIC_EOI = 0x80B,
    MSR_X2APIC_LDR = 0x80D,
    MSR_X2APIC_SIVR = 0x80F,
    MSR_X2APIC_ISR0 = 0x810,
    MSR_X2APIC_ISR1 = 0x811,
    MSR_X2APIC_ISR2 = 0x812,
    MSR_X2APIC_ISR3 = 0x813,
    MSR_X2APIC_ISR4 = 0x814,
    MSR_X2APIC_ISR5 = 0x815,
    MSR_X2APIC_ISR6 = 0x816,
    MSR_X2APIC_ISR7 = 0x817,
    MSR_X2APIC_TMR0 = 0x818,
    MSR_X2APIC_TMR1 = 0x819,
    MSR_X2APIC_TMR2 = 0x81A,
    MSR_X2APIC_TMR3 = 0x81B,
    MSR_X2APIC_TMR4 = 0x81C,
    MSR_X2APIC_TMR5 = 0x81D,
    MSR_X2APIC_TMR6 = 0x81E,
    MSR_X2APIC_TMR7 = 0x81F,
    MSR_X2APIC_IRR0 = 0x820,
    MSR_X2APIC_IRR1 = 0x821,
    MSR_X2APIC_IRR2 = 0x822,
    MSR_X2APIC_IRR3 = 0x823,
    MSR_X2APIC_IRR4 = 0x824,
    MSR_X2APIC_IRR5 = 0x825,
    MSR_X2APIC_IRR6 = 0x826,
    MSR_X2APIC_IRR7 = 0x827,
    MSR_X2APIC_ESR = 0x828,
    MSR_X2APIC_LVT_CMCI = 0x82F,
    MSR_X2APIC_ICR = 0x830,
    MSR_X2APIC_LVT_TIMER = 0x832,
    MSR_X2APIC_LVT_THERMAL = 0x833,
    MSR_X2APIC_LVT_PMI = 0x834,
    MSR_X2APIC_LVT_LINT0 = 0x835,
    MSR_X2APIC_LVT_LINT1 = 0x836,
    MSR_X2APIC_LVT_ERROR = 0x837,
    MSR_X2APIC_INIT_COUNT = 0x838,
    MSR_X2APIC_CUR_COUNT = 0x839,
    MSR_X2APIC_DIV_CONF = 0x83E,
    MSR_X2APIC_SELF_IPI = 0x83F,
    MSR_DEBUG_INTERFACE = 0xC80,
    MSR_L3_QOS_CFG = 0xC81,
    MSR_L2_QOS_CFG = 0xC82,
    MSR_QM_EVTSEL = 0xC8D,
    MSR_QM_CTR = 0xC8E,
    MSR_PQR_ASSOC = 0xC8F,
    MSR_L3_MASK_0 = 0xC90,
    MSR_L2_MASK_0 = 0xD10,
    MSR_BNDCFGS = 0xD90,
    MSR_XSS = 0xDA0,
    MSR_PKG_HDC_CTL = 0xDB0,
    MSR_PM_CTL1 = 0xDB1,
    MSR_THREAD_STALL = 0xDB2,
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
    MSR_TSC_AUX = 0xC0000103,
    MSR_CR_PAT = 0x00000277,
    MSR_CR_PAT_RESET = 0x0007040600070406ULL

};

static inline uint64_t rdmsr(uint32_t msr)
{
    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"(msr)
                 : "memory");
    return ((uint64_t)low) | (((uint64_t)high) << 32);
}

static inline void wrmsr(uint32_t msr, uint64_t Value)
{
    uint32_t low = Value, high = Value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"(msr), "a"(low), "d"(high)
                 : "memory");
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

static inline void stac()
{
    asm volatile("stac"
                 :
                 :
                 : "cc");
}

static inline void clac()
{
    asm volatile("clac"
                 :
                 :
                 : "cc");
}
