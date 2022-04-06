#include "cpuid.h"
#include <asm.h>
#include <string.h>

CPU_VENDOR cpu_vendor()
{
    static CPU_VENDOR res = " unknowncpu ";
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
            info->architecture = _64;
        }
        else
        {
            info->architecture = _32;
        }
    }
    else
    {
        info->architecture = _0;
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
    // TODO: better detection of RDX or RCX
    if (rdx & feature)
        return true;
    else if (rcx & feature)
        return true;
    return false;
}

char * cpu_get_info()
{
    static char res[512] = "================ CPU INFO ================";
    uint32_t rax, rbx, rcx, rdx;
    strcat(res, "\n-> CPU Vendor: ");
    strcat(res, cpu_vendor());
    uint32_t func;
    cpuid(0x80000000, &func, &rbx, &rcx, &rdx);
    strcat(res, "\n-> CPU Name: ");
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
    strcat(res, "\n-> CPU Architecture: ");
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
    strcat(res, "\n-> CPU Temperature Sensor: ");
    if (func >= 0x80000007)
    {
        strcat(res, "true");
    }
    else
    {
        strcat(res, "false");
    }
    cpuid(0x01, &rax, &rbx, &rcx, &rdx);
    // TODO: add more cpu features
    strcat(res, "\n-> CPU Features:");
    if (rdx & CPUID_FEAT_RDX_PAE)
        strcat(res, " PAE");
    if (rdx & CPUID_FEAT_RDX_PSE)
        strcat(res, " PSE");
    if (rdx & CPUID_FEAT_RDX_APIC)
        strcat(res, " APIC");
    if (rdx & CPUID_FEAT_RDX_MTRR)
        strcat(res, " MTRR");
    if (rdx & CPUID_FEAT_RCX_MONITOR)
        strcat(res, " MONITOR");
    strcat(res, "\n-> CPU Instructions:");
    if (rdx & CPUID_FEAT_RDX_SSE)
        strcat(res, " SSE");
    if (rdx & CPUID_FEAT_RDX_SSE2)
        strcat(res, " SSE2");
    if (rcx & CPUID_FEAT_RCX_SSE3)
        strcat(res, " SSE3");
    if (rcx & CPUID_FEAT_RCX_SSSE3)
        strcat(res, " SSSE3");
    if (rcx & CPUID_FEAT_RCX_SSE4_1)
        strcat(res, " SSE41");
    if (rcx & CPUID_FEAT_RCX_SSE4_2)
        strcat(res, " SSE42");
    if (rdx & CPUID_FEAT_RDX_MSR)
        strcat(res, " MSR");
    if (rdx & CPUID_FEAT_RDX_TSC)
        strcat(res, " TSC");
    if (rcx & CPUID_FEAT_RCX_AVX)
        strcat(res, " AVX");
    if (rcx & CPUID_FEAT_RCX_RDRAND)
        strcat(res, " RDRAND");
    if (rcx & CPUID_FEAT_RCX_F16C)
        strcat(res, " F16C");
    strcat(res, "\n==========================================\n");
    return res;
}