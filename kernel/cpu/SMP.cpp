#include "smp.hpp"
#include "gdt.h"
#include "apic.hpp"
#include <asm.h>
#include <internal_task.h>

SymmetricMultiprocessing::SMP *smp = nullptr;
CPUData CPUs[MAX_CPU];

void EnableCPUFeatures()
{
    CR0 cr0 = readcr0();
    CR4 cr4 = readcr4();
    if (cpu_feature(CPUID_FEAT_RDX_SSE))
    {
        debug("Enabling SSE support...");
        cr0.EM = 0;
        cr0.MP = 1;
        cr4.OSFXSR = 1;
        cr4.OSXMMEXCPT = 1;
    }

    // enable cpu cache but... how to use it?
    cr0.NW = 0;
    cr0.CD = 0;

    debug("Checking for UMIP, SMEP & SMAP support...");

    uint32_t rax, rbx, rcx, rdx;
    char HyperVendor[13];
    cpuid(0x40000000, &rax, &rbx, &rcx, &rdx);
    memcpy(HyperVendor + 0, &rbx, 4);
    memcpy(HyperVendor + 4, &rcx, 4);
    memcpy(HyperVendor + 8, &rdx, 4);
    HyperVendor[12] = '\0';
    debug("Hyper Vendor: %s", HyperVendor);
    if (!strcmp(HyperVendor, CPUID_VENDOR_VIRTUALBOX))
    {
        err("VirtualBox Hypervisor detected, disabling UMIP, SMEP & SMAP support...");
        if (cpu_feature(CPUID_FEAT_RDX_UMIP))
        {
            fixme("Not going to enable UMIP.");
            // cr4.UMIP = 1;
        }
        if (cpu_feature(CPUID_FEAT_RDX_SMEP))
            cr4.SMEP = 1;
        if (cpu_feature(CPUID_FEAT_RDX_SMAP))
            cr4.SMAP = 1;
        writecr0(cr0);
        writecr4(cr4);
    }
    debug("Enabling PAT support...");
    wrmsr(MSR_CR_PAT, 0x6 | (0x0 << 8) | (0x1 << 16));
}

namespace SymmetricMultiprocessing
{
    SMP::SMP()
    {
        trace("Initializing symmetric multiprocessing (%d Cores)", madt->CPUCores);

        uint32_t rax, rbx, rcx, rdx;
        char HyperVendor[13];
        cpuid(0x40000000, &rax, &rbx, &rcx, &rdx);
        memcpy(HyperVendor + 0, &rbx, 4);
        memcpy(HyperVendor + 4, &rcx, 4);
        memcpy(HyperVendor + 8, &rdx, 4);
        HyperVendor[12] = '\0';
        debug("Hyper Vendor: %s", HyperVendor);
        if (!strcmp(HyperVendor, CPUID_VENDOR_VIRTUALBOX))
        {
            err("VirtualBox Hypervisor detected, disabling SMP support...");
            EnableCPUFeatures();
            return;
        }
        EnableCPUFeatures();
    }

    SMP::~SMP()
    {
    }
}
