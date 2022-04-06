#pragma once

#include <types.h>

/* DEPRECATED: You should use the <cpuid.h> header that comes with GCC instead. */

enum cpuid_requests
{
    CPUID_GETVENDORSTRING,
    CPUID_GETFEATURES,
    CPUID_GETTLB,
    CPUID_GETSERIAL,

    CPUID_INTELEXTENDED = 0x80000000,
    CPUID_INTELFEATURES,
    CPUID_INTELBRANDSTRING,
    CPUID_INTELBRANDSTRINGMORE,
    CPUID_INTELBRANDSTRINGEND,
};

enum CPU_FEATURE
{
    CPUID_FEAT_RCX_SSE3 = 1 << 0,
    CPUID_FEAT_RCX_PCLMUL = 1 << 1,
    CPUID_FEAT_RCX_DTES64 = 1 << 2,
    CPUID_FEAT_RCX_MONITOR = 1 << 3,
    CPUID_FEAT_RCX_DS_CPL = 1 << 4,
    CPUID_FEAT_RCX_VMX = 1 << 5,
    CPUID_FEAT_RCX_SMX = 1 << 6,
    CPUID_FEAT_RCX_EST = 1 << 7,
    CPUID_FEAT_RCX_TM2 = 1 << 8,
    CPUID_FEAT_RCX_SSSE3 = 1 << 9,
    CPUID_FEAT_RCX_CID = 1 << 10,
    CPUID_FEAT_RCX_FMA = 1 << 12,
    CPUID_FEAT_RCX_CX16 = 1 << 13,
    CPUID_FEAT_RCX_ETPRD = 1 << 14,
    CPUID_FEAT_RCX_PDCM = 1 << 15,
    CPUID_FEAT_RCX_PCIDE = 1 << 17,
    CPUID_FEAT_RCX_DCA = 1 << 18,
    CPUID_FEAT_RCX_SSE4_1 = 1 << 19,
    CPUID_FEAT_RCX_SSE4_2 = 1 << 20,
    CPUID_FEAT_RCX_x2APIC = 1 << 21,
    CPUID_FEAT_RCX_MOVBE = 1 << 22,
    CPUID_FEAT_RCX_POPCNT = 1 << 23,
    CPUID_FEAT_RCX_AES = 1 << 25,
    CPUID_FEAT_RCX_XSAVE = 1 << 26,
    CPUID_FEAT_RCX_OSXSAVE = 1 << 27,
    CPUID_FEAT_RCX_AVX = 1 << 28,
    CPUID_FEAT_RCX_F16C = 1 << 29,
    CPUID_FEAT_RCX_RDRAND = 1 << 30,

    CPUID_FEAT_RDX_FPU = 1 << 0,
    CPUID_FEAT_RDX_VME = 1 << 1,
    CPUID_FEAT_RDX_DE = 1 << 2,
    CPUID_FEAT_RDX_PSE = 1 << 3,
    CPUID_FEAT_RDX_TSC = 1 << 4,
    CPUID_FEAT_RDX_MSR = 1 << 5,
    CPUID_FEAT_RDX_PAE = 1 << 6,
    CPUID_FEAT_RDX_MCE = 1 << 7,
    CPUID_FEAT_RDX_CX8 = 1 << 8,
    CPUID_FEAT_RDX_APIC = 1 << 9,
    CPUID_FEAT_RDX_SEP = 1 << 11,
    CPUID_FEAT_RDX_MTRR = 1 << 12,
    CPUID_FEAT_RDX_PGE = 1 << 13,
    CPUID_FEAT_RDX_MCA = 1 << 14,
    CPUID_FEAT_RDX_CMOV = 1 << 15,
    CPUID_FEAT_RDX_PAT = 1 << 16,
    CPUID_FEAT_RDX_PSE36 = 1 << 17,
    CPUID_FEAT_RDX_PSN = 1 << 18,
    CPUID_FEAT_RDX_CLF = 1 << 19,
    CPUID_FEAT_RDX_DTES = 1 << 21,
    CPUID_FEAT_RDX_ACPI = 1 << 22,
    CPUID_FEAT_RDX_MMX = 1 << 23,
    CPUID_FEAT_RDX_FXSR = 1 << 24,
    CPUID_FEAT_RDX_SSE = 1 << 25,
    CPUID_FEAT_RDX_SSE2 = 1 << 26,
    CPUID_FEAT_RDX_SS = 1 << 27,
    CPUID_FEAT_RDX_HTT = 1 << 28,
    CPUID_FEAT_RDX_TM1 = 1 << 29,
    CPUID_FEAT_RDX_IA64 = 1 << 30,
    CPUID_FEAT_RDX_PBE = 1 << 31,

    CPUID_FEAT_RDX_SMEP = 1 << 7,
    CPUID_FEAT_RDX_SYSCALL = 1 << 11,
    CPUID_FEAT_XD = 1 << 20,
    CPUID_FEAT_1GB_PAGE = 1 << 26,
    CPUID_FEAT_RDTSCP = 1 << 27,
    CPUID_FEAT_LONG_MODE = 1 << 29,
    CPUID_FEAT_RDX_SMAP = 0x00100000
};

// https://wiki.osdev.org/CPUID

/**
 * @brief Char array that should be 12 or 13 characters
 * 
 */
typedef char *CPU_VENDOR;

#define CPUID_VENDOR_OLDAMD "AMDisbetter!" /* early engineering samples of AMD K5 processor */
#define CPUID_VENDOR_AMD "AuthenticAMD"
#define CPUID_VENDOR_INTEL "GenuineIntel"
#define CPUID_VENDOR_VIA "CentaurHauls"
#define CPUID_VENDOR_OLDTRANSMETA "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA "GenuineTMx86"
#define CPUID_VENDOR_CYRIX "CyrixInstead"
#define CPUID_VENDOR_CENTAUR "CentaurHauls"
#define CPUID_VENDOR_NEXGEN "NexGenDriven"
#define CPUID_VENDOR_UMC "UMC UMC UMC "
#define CPUID_VENDOR_SIS "SiS SiS SiS "
#define CPUID_VENDOR_NSC "Geode by NSC"
#define CPUID_VENDOR_RISE "RiseRiseRise"
#define CPUID_VENDOR_VORTEX "Vortex86 SoC"
#define CPUID_VENDOR_VIA2 "VIA VIA VIA "

/*Vendor-strings from Virtual Machines.*/
#define CPUID_VENDOR_VMWARE "VMwareVMware"
#define CPUID_VENDOR_XENHVM "XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS " lrpepyh vr"

enum CPU_ARCHITECTURE
{
    _0 = 0,
    _8 = 8,
    _16 = 16,
    _32 = 32,
    _64 = 64,
    /**
     * @brief haha, I have to see this!
     */
    _128 = 128
};

typedef struct _CPU_INFO
{
    CPU_VENDOR vendor;
    char *name;
    enum CPU_ARCHITECTURE architecture;
    bool temperature_sensor;
    enum CPU_FEATURE feature;
} CPU_INFO;

static inline int cpuid_string(int code, int where[4])
{
    asm volatile("cpuid"
                 : "=a"(*where), "=b"(*(where + 0)),
                   "=d"(*(where + 1)), "=c"(*(where + 2))
                 : "a"(code));
    return (int)where[0];
}

CPU_VENDOR cpu_vendor();
/**
 * @brief Check if the specified CPU feature is supported.
 * 
 * @param feature The CPU feature to check.
 * @return bool
 */
EXTERNC bool cpu_feature(enum CPU_FEATURE feature);
char * cpu_get_info();
