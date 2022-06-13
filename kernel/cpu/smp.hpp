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

// TODO: a better approach is to get lapic id instead of storing the ID in the FS register
static CPUData *GetCurrentCPU()
{
    uint64_t ret = 0;
    // asm volatile("movq %%fs, %0\n"
    //              : "=r"(ret));

    if ((&CPUs[ret])->Checksum != CPU_DATA_CHECKSUM)
    {
        // TODO: i think somehow i messed this up somehere... i'll figure it out later... but now i will return the first cpu
        err("CPU %d data are corrupted!", ret);
        return &CPUs[0];
    }

    return &CPUs[ret];
}

static CPUData *GetCPU(uint64_t id) { return &CPUs[id]; }

#define CurrentCPU GetCurrentCPU()
