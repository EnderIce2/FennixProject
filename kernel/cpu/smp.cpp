#include "smp.hpp"
#include "gdt.h"
#include <asm.h>

SymmetricMultiprocessing::SMP *smp = nullptr;
SymmetricMultiprocessing::SMP::CPUData *CPUs = nullptr;

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
    // Not really useful at the moment and is causing a general protection fault (maybe because i don't know how to use it properly).
    // debug("Checking for UMIP, SMEP & SMAP support...");
    // if (cpu_feature(CPUID_FEAT_RDX_UMIP))
    //     cr4.UMIP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMEP))
    //     cr4.SMEP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMAP))
    //     cr4.SMAP = 1;
    writecr0(cr0);
    writecr4(cr4);
    debug("Enabling PAT support...");
    wrmsr(MSR_CR_PAT, 0x6 | (0x0 << 8) | (0x1 << 16));
}

static void InitializeCPU(GBPSMPInfo *Data)
{
    if (Data->LAPICID != bootparams->smp.BootstrapProcessLAPICID)
        asm volatile("mov %%rsp, %0"
                     : "=r"(tss[((SymmetricMultiprocessing::SMP::CPUData *)Data->ExtraArgument)->ID].StackPointer0)
                     :
                     : "memory");

    wrmsr(MSR_SHADOW_GS_BASE, Data->ExtraArgument);
    wrmsr(MSR_GS_BASE, Data->ExtraArgument);

    tss = (TaskStateSegment *)kcalloc(bootparams->smp.CPUCount, sizeof(TaskStateSegment));
    uint64_t tss_base = (uint64_t)&tss[Data->ID];
    gdt.Entries->TaskStateSegment.Length = tss_base + sizeof(tss[Data->ID]);
    gdt.Entries->TaskStateSegment.Low = (uint16_t)(tss_base & 0xFFFF);
    gdt.Entries->TaskStateSegment.Middle = (uint8_t)((tss_base >> 16) & 0xFF);
    gdt.Entries->TaskStateSegment.High = (uint8_t)((tss_base >> 24) & 0xFF);
    gdt.Entries->TaskStateSegment.Upper32 = (uint32_t)((tss_base >> 32) & 0xFFFFFFFF);
    gdt.Entries->TaskStateSegment.Flags1 = 0b10001001;
    gdt.Entries->TaskStateSegment.Flags2 = 0b00000000;
    (&tss[Data->ID])->IOMapBaseAddressOffset = sizeof(TaskStateSegment);
    ltr(GDT_TSS);
    (&tss[Data->ID])->StackPointer0 = (uint64_t)kernel_stack;
    (&tss[Data->ID])->InterruptStackTable0 = (uint64_t)RequestPage(); // exceptions
    (&tss[Data->ID])->InterruptStackTable1 = (uint64_t)RequestPage(); // nmi
    (&tss[Data->ID])->InterruptStackTable2 = (uint64_t)RequestPage(); // page fault, double fault, general protection fault, etc...
    lgdt(gdt);
    lidt(idtr);

    CR3 cr3;
    cr3.raw = (uint64_t)KernelPageTableManager.PML4;
    writecr3(cr3);

    CurrentProcessor->LAPICID = Data->LAPICID;
    CurrentProcessor->TSS = &tss[CurrentProcessor->ID];
    EnableCPUFeatures();

    // TODO: fxsr, syscalls, apic

    CurrentProcessor->Ready = true;
    trace("CPU %d Ready", Data->ID);
}

namespace SymmetricMultiprocessing
{
    SMP::SMP()
    {
        trace("Initializing symmetric multiprocessing (%d Cores)", bootparams->smp.CPUCount);
        CPUs = new SMP::CPUData[bootparams->smp.CPUCount];
        for (size_t i = 0; i < bootparams->smp.CPUCount; i++)
        {
            bootparams->smp.smp[i].ExtraArgument = (uint64_t)&CPUs[i];
            CPUs[i].ID = bootparams->smp.smp[i].ID;
            (&tss[i])->InterruptStackTable0 = (uint64_t)KernelAllocator.RequestPage();

            if (bootparams->smp.BootstrapProcessLAPICID == bootparams->smp.smp[i].LAPICID)
                InitializeCPU(&bootparams->smp.smp[i]);
            else
            {
                // TODO: implement ap trampoline - https://wiki.osdev.org/Symmetric_Multiprocessing
                // trace("Waiting for CPU %d to be ready...", CPUs[i].ID);
                // while (!CPUs[i].Ready)
                //     asm("pause");
            }
        }
    }

    SMP::~SMP()
    {
    }
}
