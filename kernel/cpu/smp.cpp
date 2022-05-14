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
        // debug("Enabling SSE support...");
        cr0.EM = 0;
        cr0.MP = 1;
        cr4.OSFXSR = 1;
        cr4.OSXMMEXCPT = 1;
    }

    // enable cpu cache but... how to use it?
    cr0.NW = 0;
    cr0.CD = 0;


    // Not really useful at the moment and is causing a general protection fault (maybe because i don't know how to use it properly).
    // debug("Checking for UMIP, SMEP & SMAP support...");
    // if (cpu_feature(CPUID_FEAT_RDX_UMIP))
    //     cr4.UMIP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMEP))
    //     cr4.SMEP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMAP))
        // cr4.SMAP = 1;
    writecr0(cr0);
    writecr4(cr4);
    // debug("Enabling PAT support...");
    wrmsr(MSR_CR_PAT, 0x6 | (0x0 << 8) | (0x1 << 16));
}

// Inspired from https://github.com/Supercip971/WingOS/blob/master/kernel/arch/x86_64/smp.cpp

extern "C" uint32_t _trampoline_start, _trampoline_end, nstack;

volatile bool CPUEnabled = false;

extern "C" void StartCPU()
{
    EnableCPUFeatures();
    wrmsr(MSR_APIC, (rdmsr(MSR_APIC) | 0x800) & ~(1 << 10));
    apic->Write(APIC::APIC::APIC_SVR, apic->Read(APIC::APIC::APIC_SVR) | 0x1FF);

    uint64_t apicid = apic->Read(APIC::APIC::APIC_ID) >> 24;

    // asm volatile("mov %0, %%fs" ::"r"(apicid));
    debug("Loading CPU %d...", apicid);
    // create a new gdt for the cpu
    asm("cli");

    init_syscalls();
    wrmsr(MSR_GS_BASE, (uintptr_t)CurrentCPU);
    wrmsr(MSR_FS_BASE, apicid);
    wrmsr(MSR_SHADOW_GS_BASE, (uintptr_t)CurrentCPU);

    asm("sti");
    CPUEnabled = true;
    CPU_STOP;
}

#define TRAMPOLINE_START 0x2000

enum smpaddresses
{
    PAGE_TABLE = 0x500,
    START_ADDR = 0x520,
    _STACK = 0x570,
    GDT = 0x580,
    IDT = 0x590,
};

static void InitializeCPU(ACPI::MADT::LocalAPIC *lapic)
{
    apic->Write(APIC::APIC::APIC_ICRHI, (lapic->APICId << 24));
    apic->Write(APIC::APIC::APIC_ICRLO, 0x500);

    KernelPageTableManager.MapMemory(0x0, 0x0, PTFlag::RW | PTFlag::US);

    uint64_t trampoline_len = (uintptr_t)&_trampoline_end - (uintptr_t)&_trampoline_start;
    for (uint64_t i = 0; i < (trampoline_len / PAGE_SIZE) + 2; i++)
        KernelPageTableManager.MapMemory((void *)(TRAMPOLINE_START + (i * PAGE_SIZE)), (void *)(TRAMPOLINE_START + (i * PAGE_SIZE)), PTFlag::RW | PTFlag::US);

    memcpy((void *)TRAMPOLINE_START, &_trampoline_start, trampoline_len);

    GetCPU(lapic->APICId)->PageTable = GetCurrentCPU()->PageTable;
    // POKE(volatile uint64_t, PAGE_TABLE) = GetCPU(lapic->APICId)->PageTable.raw;
    POKE(volatile uint64_t, PAGE_TABLE) = readcr3().raw;
    memset(GetCPU(lapic->APICId)->Stack, 0, STACK_SIZE);

    // POKE(volatile uint64_t, _STACK) = (uint64_t)GetCPU(lapic->APICId)->Stack + STACK_SIZE;
    POKE(volatile uint64_t, _STACK) = (uint64_t)KernelAllocator.RequestPage();

    asm volatile(" \n"
                 "sgdt [0x580]\n"
                 "sidt [0x590]\n");

    // start address at 0x520
    POKE(volatile uint64_t, START_ADDR) = (uintptr_t)&StartCPU;

    apic->Write(APIC::APIC::APIC_ICRHI, (lapic->APICId << 24));
    apic->Write(APIC::APIC::APIC_ICRLO, 0x600 | ((uint32_t)TRAMPOLINE_START / PAGE_SIZE));

    while (!CPUEnabled)
        ;

    trace("CPU %d loaded.", lapic->APICId);

    CPUEnabled = false;

    // tss = (TaskStateSegment *)kcalloc(bootparams->smp.CPUCount, sizeof(TaskStateSegment));
    // uint64_t tss_base = (uint64_t)&tss[lapic->APICId];
    // gdt.Entries->TaskStateSegment.Length = tss_base + sizeof(tss[lapic->APICId]);
    // gdt.Entries->TaskStateSegment.Low = (uint16_t)(tss_base & 0xFFFF);
    // gdt.Entries->TaskStateSegment.Middle = (uint8_t)((tss_base >> 16) & 0xFF);
    // gdt.Entries->TaskStateSegment.High = (uint8_t)((tss_base >> 24) & 0xFF);
    // gdt.Entries->TaskStateSegment.Upper32 = (uint32_t)((tss_base >> 32) & 0xFFFFFFFF);
    // gdt.Entries->TaskStateSegment.Flags1 = 0b10001001;
    // gdt.Entries->TaskStateSegment.Flags2 = 0b00000000;
    // (&tss[lapic->APICId])->IOMapBaseAddressOffset = sizeof(TaskStateSegment);
    // ltr(GDT_TSS);
    // (&tss[lapic->APICId])->StackPointer0 = (uint64_t)kernel_stack;
    // (&tss[lapic->APICId])->InterruptStackTable0 = (uint64_t)RequestPage(); // exceptions
    // (&tss[lapic->APICId])->InterruptStackTable1 = (uint64_t)RequestPage(); // nmi
    // (&tss[lapic->APICId])->InterruptStackTable2 = (uint64_t)RequestPage(); // page fault, double fault, general protection fault, etc...
    // lgdt(gdt);
    // lidt(idtr);
    // CR3 cr3;
    // cr3.raw = (uint64_t)KernelPageTableManager.PML4;
    // writecr3(cr3);
    // CurrentProcessor->LAPICID = lapic->APICId;
    // CurrentProcessor->TSS = &tss[CurrentProcessor->ID];
    // TODO: fxsr, syscalls, apic
    // CurrentProcessor->Ready = true;
    // trace("CPU %d Ready", lapic->APICId);
}

namespace SymmetricMultiprocessing
{
    SMP::SMP()
    {
        trace("Initializing symmetric multiprocessing (%d Cores)", madt->CPUCores);

        for (size_t i = 0; i < madt->CPUCores; i++)
            if ((apic->Read(APIC::APIC::APIC_ID) >> 24) != madt->lapic[i]->ACPIProcessorId)
                InitializeCPU(madt->lapic[i]);
            else
                EnableCPUFeatures();
    }

    SMP::~SMP()
    {
    }
}
