#include "cpuid.h"
#include <asm.h>
#include <string.h>
#include <heap.h>

CPU_VENDOR cpu_vendor()
{
    CPU_VENDOR res = (char *)kmalloc(sizeof(12));
    cpuid_string(0, (int *)(res));
    return res;
}

CPU_INFO *GetCPUInfo()
{
    CPU_INFO *info = NULL;
    uint32_t rax, rbx, rcx, rdx;
    uint32_t func;
    info->vendor = cpu_vendor();
    cpuid(0x80000000, &func, &rbx, &rcx, &rdx);
    if (func >= 0x80000004)
    {
        char name_cpuid[48];
        cpuid(0x80000002, (uint32_t *)(name_cpuid + 0), (uint32_t *)(name_cpuid + 4), (uint32_t *)(name_cpuid + 8), (uint32_t *)(name_cpuid + 12));
        cpuid(0x80000003, (uint32_t *)(name_cpuid + 16), (uint32_t *)(name_cpuid + 20), (uint32_t *)(name_cpuid + 24), (uint32_t *)(name_cpuid + 28));
        cpuid(0x80000004, (uint32_t *)(name_cpuid + 32), (uint32_t *)(name_cpuid + 36), (uint32_t *)(name_cpuid + 40), (uint32_t *)(name_cpuid + 44));
        char *space = name_cpuid;
        while (*space == ' ')
        {
            ++space;
        }
        info->name = space;
    }
    else
    {
        info->name = "Unknown";
    }
    if (func >= 0x80000001)
    {
        cpuid(0x80000001, &rax, &rbx, &rcx, &rdx);
        if (rdx & CPUID_FEAT_LONG_MODE)
        {
            info->architecture = x64;
        }
        else
        {
            info->architecture = x32;
        }
    }
    else
    {
        info->architecture = x0;
    }
    // TODO: add multiple sensor detections
    if (func >= 0x80000007)
    {
        info->temperature_sensor = true;
    }
    else
    {
        info->temperature_sensor = false;
    }
    cpuid(0x01, &rax, &rbx, &rcx, &rdx);
    for (enum CPU_FEATURE i = CPUID_FEAT_RCX_SSE3; i < CPUID_FEAT_RCX_RDRAND; i++)
    {
        if (rcx & i)
            info->feature = i; // TODO: this really works???????
    }
    for (enum CPU_FEATURE i = CPUID_FEAT_RDX_FPU; i < CPUID_FEAT_RDX_PBE; i++)
    {
        if (rdx & i)
            info->feature = i; // TODO: this really works???????
    }
    return 0;
}

bool cpu_feature(enum CPU_FEATURE feature)
{
    uint32_t rax, rbx, rcx, rdx;
    cpuid(0x01, &rax, &rbx, &rcx, &rdx);
    if (rdx & feature || rcx & feature)
        return true;
    return false;
}

char *cpu_get_info()
{
    static char res[512] = "\0";
    uint32_t rax, rbx, rcx, rdx;
    strcat(res, "CPU Vendor: ");
    strcat(res, cpu_vendor());
    uint32_t func;
    cpuid(0x80000000, &func, &rbx, &rcx, &rdx);
    strcat(res, "\nCPU Name: ");
    if (func >= 0x80000004)
    {
        char name_cpuid[48];
        cpuid(0x80000002, (uint32_t *)(name_cpuid + 0), (uint32_t *)(name_cpuid + 4), (uint32_t *)(name_cpuid + 8), (uint32_t *)(name_cpuid + 12));
        cpuid(0x80000003, (uint32_t *)(name_cpuid + 16), (uint32_t *)(name_cpuid + 20), (uint32_t *)(name_cpuid + 24), (uint32_t *)(name_cpuid + 28));
        cpuid(0x80000004, (uint32_t *)(name_cpuid + 32), (uint32_t *)(name_cpuid + 36), (uint32_t *)(name_cpuid + 40), (uint32_t *)(name_cpuid + 44));
        const char *space = name_cpuid;
        while (*space == ' ')
        {
            ++space;
        }
        strcat(res, space);
    }
    else
    {
        strcat(res, "unknown");
    }
    strcat(res, "\nCPU Architecture: ");
    if (func >= 0x80000001)
    {
        cpuid(0x80000001, &rax, &rbx, &rcx, &rdx);
        if (rdx & CPUID_FEAT_LONG_MODE)
        {
            strcat(res, "64-bit");
        }
        else
        {
            strcat(res, "32-bit");
        }
    }
    else
    {
        strcat(res, "unknown");
    }
    // TODO: add multiple sensor detections
    strcat(res, "\nCPU Temperature Sensor: ");
    if (func >= 0x80000007)
    {
        strcat(res, "true");
    }
    else
    {
        strcat(res, "false");
    }
    cpuid(0x01, &rax, &rbx, &rcx, &rdx);
    strcat(res, "\nSEE:");

    if (rdx & CPUID_FEAT_RDX_SSE)
        strcat(res, " SSE");
    if (rdx & CPUID_FEAT_RDX_SSE2)
        strcat(res, " SSE2");
    if (rcx & CPUID_FEAT_RCX_SSE3)
        strcat(res, " SSE3");
    if (rcx & CPUID_FEAT_RCX_SSSE3)
        strcat(res, " SSSE3");
    if (rcx & CPUID_FEAT_RCX_SSE4_1)
        strcat(res, " SSE4_1");
    if (rcx & CPUID_FEAT_RCX_SSE4_2)
        strcat(res, " SSE4_2");

    strcat(res, "\nRCX:");

    if (rcx & CPUID_FEAT_RCX_PCLMULQDQ)
        strcat(res, " PCLMULQDQ");
    if (rcx & CPUID_FEAT_RCX_DTES64)
        strcat(res, " DTES64");
    if (rcx & CPUID_FEAT_RCX_MONITOR)
        strcat(res, " MONITOR");
    if (rcx & CPUID_FEAT_RCX_DS_CPL)
        strcat(res, " DS_CPL");
    if (rcx & CPUID_FEAT_RCX_VMX)
        strcat(res, " VMX");
    if (rcx & CPUID_FEAT_RCX_SMX)
        strcat(res, " SMX");
    if (rcx & CPUID_FEAT_RCX_EST)
        strcat(res, " EST");
    if (rcx & CPUID_FEAT_RCX_TM2)
        strcat(res, " TM2");
    if (rcx & CPUID_FEAT_RCX_CID)
        strcat(res, " CID");
    if (rcx & CPUID_FEAT_RCX_FMA)
        strcat(res, " FMA");
    if (rcx & CPUID_FEAT_RCX_CX16)
        strcat(res, " CX16");
    if (rcx & CPUID_FEAT_RCX_ETPRD)
        strcat(res, " ETPRD");
    if (rcx & CPUID_FEAT_RCX_PDCM)
        strcat(res, " PDCM");
    if (rcx & CPUID_FEAT_RCX_PCIDE)
        strcat(res, " PCIDE");
    if (rcx & CPUID_FEAT_RCX_DCA)
        strcat(res, " DCA");
    if (rcx & CPUID_FEAT_RCX_x2APIC)
        strcat(res, " x2APIC");
    if (rcx & CPUID_FEAT_RCX_MOVBE)
        strcat(res, " MOVBE");
    if (rcx & CPUID_FEAT_RCX_POPCNT)
        strcat(res, " POPCNT");
    if (rcx & CPUID_FEAT_RCX_AES)
        strcat(res, " AES");
    if (rcx & CPUID_FEAT_RCX_XSAVE)
        strcat(res, " XSAVE");
    if (rcx & CPUID_FEAT_RCX_OSXSAVE)
        strcat(res, " OSXSAVE");
    if (rcx & CPUID_FEAT_RCX_AVX)
        strcat(res, " AVX");
    if (rcx & CPUID_FEAT_RCX_F16C)
        strcat(res, " F16C");
    if (rcx & CPUID_FEAT_RCX_RDRAND)
        strcat(res, " RDRAND");

    strcat(res, "\nRDX:");

    if (rdx & CPUID_FEAT_RDX_FPU)
        strcat(res, " FPU");
    if (rdx & CPUID_FEAT_RDX_VME)
        strcat(res, " VME");
    if (rdx & CPUID_FEAT_RDX_DE)
        strcat(res, " DE");
    if (rdx & CPUID_FEAT_RDX_PSE)
        strcat(res, " PSE");
    if (rdx & CPUID_FEAT_RDX_TSC)
        strcat(res, " TSC");
    if (rdx & CPUID_FEAT_RDX_MSR)
        strcat(res, " MSR");
    if (rdx & CPUID_FEAT_RDX_PAE)
        strcat(res, " PAE");
    if (rdx & CPUID_FEAT_RDX_MCE)
        strcat(res, " MCE");
    if (rdx & CPUID_FEAT_RDX_CX8)
        strcat(res, " CX8");
    if (rdx & CPUID_FEAT_RDX_APIC)
        strcat(res, " APIC");
    if (rdx & CPUID_FEAT_RDX_SEP)
        strcat(res, " SEP");
    if (rdx & CPUID_FEAT_RDX_MTRR)
        strcat(res, " MTRR");
    if (rdx & CPUID_FEAT_RDX_PGE)
        strcat(res, " PGE");
    if (rdx & CPUID_FEAT_RDX_MCA)
        strcat(res, " MCA");
    if (rdx & CPUID_FEAT_RDX_CMOV)
        strcat(res, " CMOV");
    if (rdx & CPUID_FEAT_RDX_PAT)
        strcat(res, " PAT");
    if (rdx & CPUID_FEAT_RDX_PSE36)
        strcat(res, " PSE36");
    if (rdx & CPUID_FEAT_RDX_PSN)
        strcat(res, " PSN");
    if (rdx & CPUID_FEAT_RDX_CLF)
        strcat(res, " CLF");
    if (rdx & CPUID_FEAT_RDX_DTES)
        strcat(res, " DTES");
    if (rdx & CPUID_FEAT_RDX_ACPI)
        strcat(res, " ACPI");
    if (rdx & CPUID_FEAT_RDX_MMX)
        strcat(res, " MMX");
    if (rdx & CPUID_FEAT_RDX_FXSR)
        strcat(res, " FXSR");
    if (rdx & CPUID_FEAT_RDX_SS)
        strcat(res, " SS");
    if (rdx & CPUID_FEAT_RDX_HTT)
        strcat(res, " HTT");
    if (rdx & CPUID_FEAT_RDX_TM1)
        strcat(res, " TM1");
    if (rdx & CPUID_FEAT_RDX_IA64)
        strcat(res, " IA64");
    if (rdx & CPUID_FEAT_RDX_PBE)
        strcat(res, " PBE");
    if (rdx & CPUID_FEAT_RDX_SMEP)
        strcat(res, " SMEP");
    if (rdx & CPUID_FEAT_RDX_SMAP)
        strcat(res, " SMAP");

    strcat(res, "\n");
    return res;
}