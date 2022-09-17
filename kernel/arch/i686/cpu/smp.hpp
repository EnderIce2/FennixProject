#pragma once
#include <task.h>

#include "../kernel.h"
#include "acpi.hpp"
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

    long ErrorCode;
    unsigned int Checksum = CPU_DATA_CHECKSUM;

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

int GetCurrentCPUID();

static CPUData *GetCurrentCPU()
{
    uint64_t ret = GetCurrentCPUID();

    if (CPUs[ret].Checksum != CPU_DATA_CHECKSUM)
    {
        // TODO: i think somehow i messed this up somehere... i'll figure it out later... but now i will return the first cpu
        err("CPU %d data is corrupted!", ret);
        return &CPUs[0];
    }
    return &CPUs[ret];
}

static CPUData *GetCPU(uint64_t id) { return &CPUs[id]; }

#define CurrentCPU GetCurrentCPU()