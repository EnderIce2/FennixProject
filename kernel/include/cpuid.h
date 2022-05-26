#pragma once

enum CPU_FEATURE
{
    CPUID_FEAT_RCX_SSE3 = 1 << 0,
    CPUID_FEAT_RCX_PCLMULQDQ = 1 << 1,
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

    // ? Not sure how to get it.
    CPUID_FEAT_RDX_SMEP = 1 << 7,
    CPUID_FEAT_RDX_UMIP = 1 << 2,
    CPUID_FEAT_RDX_SYSCALL = 1 << 11,
    CPUID_FEAT_XD = 1 << 20,
    CPUID_FEAT_1GB_PAGE = 1 << 26,
    CPUID_FEAT_RDTSCP = 1 << 27,
    CPUID_FEAT_LONG_MODE = 1 << 29,
    CPUID_FEAT_RDX_SMAP = (1 << 20)
};

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

/* Vendor-strings from Virtual Machines. */
#define CPUID_VENDOR_VMWARE "VMwareVMware"
#define CPUID_VENDOR_XENHVM "XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS " lrpepyh vr"
#define CPUID_VENDOR_KVM "KVMKVMKVM"
#define CPUID_VENDOR_VIRTUALBOX "VBoxVBoxVBox"
#define CPUID_VENDOR_TCG "TCGTCGTCGTCG"

#define SIGNATURE_INTEL_b 0x756e6547
#define SIGNATURE_INTEL_c 0x6c65746e
#define SIGNATURE_INTEL_d 0x49656e69

#define SIGNATURE_AMD_b 0x68747541
#define SIGNATURE_AMD_c 0x444d4163
#define SIGNATURE_AMD_d 0x69746e65

#define SIGNATURE_CENTAUR_b 0x746e6543
#define SIGNATURE_CENTAUR_c 0x736c7561
#define SIGNATURE_CENTAUR_d 0x48727561

#define SIGNATURE_CYRIX_b 0x69727943
#define SIGNATURE_CYRIX_c 0x64616574
#define SIGNATURE_CYRIX_d 0x736e4978

#define SIGNATURE_TM1_b 0x6e617254
#define SIGNATURE_TM1_c 0x55504361
#define SIGNATURE_TM1_d 0x74656d73

#define SIGNATURE_TM2_b 0x756e6547
#define SIGNATURE_TM2_c 0x3638784d
#define SIGNATURE_TM2_d 0x54656e69

#define SIGNATURE_NSC_b 0x646f6547
#define SIGNATURE_NSC_c 0x43534e20
#define SIGNATURE_NSC_d 0x79622065

#define SIGNATURE_NEXGEN_b 0x4778654e
#define SIGNATURE_NEXGEN_c 0x6e657669
#define SIGNATURE_NEXGEN_d 0x72446e65

#define SIGNATURE_RISE_b 0x65736952
#define SIGNATURE_RISE_c 0x65736952
#define SIGNATURE_RISE_d 0x65736952

#define SIGNATURE_SIS_b 0x20536953
#define SIGNATURE_SIS_c 0x20536953
#define SIGNATURE_SIS_d 0x20536953

#define SIGNATURE_UMC_b 0x20434d55
#define SIGNATURE_UMC_c 0x20434d55
#define SIGNATURE_UMC_d 0x20434d55

#define SIGNATURE_VIA_b 0x20414956
#define SIGNATURE_VIA_c 0x20414956
#define SIGNATURE_VIA_d 0x20414956

#define SIGNATURE_VORTEX_b 0x74726f56
#define SIGNATURE_VORTEX_c 0x436f5320
#define SIGNATURE_VORTEX_d 0x36387865

#ifndef __x86_64__

#define __cpuid(level, a, b, c, d)                       \
    do                                                   \
    {                                                    \
        if (__builtin_constant_p(level) && (level) != 1) \
            __asm__ volatile("cpuid\n\t"                 \
                             : "=a"(a),                  \
                               "=b"(b),                  \
                               "=c"(c),                  \
                               "=d"(d)                   \
                             : "0"(level));              \
        else                                             \
            __asm__ volatile("cpuid\n\t"                 \
                             : "=a"(a),                  \
                               "=b"(b),                  \
                               "=c"(c),                  \
                               "=d"(d)                   \
                             : "0"(level),               \
                               "1"(0),                   \
                               "2"(0));                  \
    } while (0)

#else

#define __cpuid(level, a, b, c, d) \
    __asm__ volatile("cpuid\n\t"   \
                     : "=a"(a),    \
                       "=b"(b),    \
                       "=c"(c),    \
                       "=d"(d)     \
                     : "0"(level))

#endif
