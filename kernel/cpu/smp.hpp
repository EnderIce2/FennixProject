#pragma once
#include "idt.h"
#include "acpi.hpp"
#include "../kernel.h"

struct CPUData
{
    uint64_t ID;
    ACPI::MADT::LocalAPIC LAPIC;
    uint8_t Stack[STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
    CR3 PageTable;
    GlobalDescriptorTableDescriptor GDT;
    InterruptDescriptorTableDescriptor IDT;
    TaskStateSegment TSS;
};

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
    uint64_t ret = 0;
    asm volatile("movq %%fs, %0\n"
                 : "=r"(ret));
    return &CPUs[ret];
}

static CPUData *GetCPU(uint64_t id) { return &CPUs[id]; }

#define CurrentCPU GetCurrentCPU()
