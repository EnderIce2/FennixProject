#pragma once
#include <task.h>

#include "../kernel.h"
#include "acpi.hpp"
#include "fxsr.h"
#include "idt.h"

#define CPU_DATA_CHECKSUM 0xdeadda7a

struct CPUData
{
    uint64_t ID;
    ACPI::MADT::LocalAPIC LAPIC;
    CR3 PageTable;
    GlobalDescriptorTableDescriptor GDT;
    InterruptDescriptorTableDescriptor IDT;
    TaskStateSegment TSS;

    PCB *CurrentProcess;
    TCB *CurrentThread;

    void fxsave(char *Buffer) { _fxsave(Buffer); }
    void fxrstor(char *Buffer) { _fxrstor(Buffer); }

    unsigned int Checksum;

    uint8_t Stack[STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
} __attribute__((packed));

namespace SymmetricMultiprocessing
{
    class SMP
    {
    public:
        SMP();
        ~SMP();
    };
}

extern SymmetricMultiprocessing::SMP *smp;

#define MAX_CPU 256
extern CPUData CPUs[];

static CPUData *GetCurrentCPU()
{
    if ((&CPUs[0])->Checksum != CPU_DATA_CHECKSUM)
        err("CPU %d data are corrupted!", 0);
    return &CPUs[0];
}

static CPUData *GetCPU(uint64_t id) { return &CPUs[id]; }

#define CurrentCPU GetCurrentCPU()
