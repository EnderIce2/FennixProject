#pragma once
#include <task.h>

#include "../kernel.h"
#include "acpi.hpp"
#include "fxsr.h"
#include "idt.h"

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
    uint64_t ret = 0;
    asm volatile("movq %%fs, %0\n"
                 : "=r"(ret));
    return &CPUs[ret];
}

static uint64_t GetCurrentCPUID()
{
    uint64_t ret = 0;
    asm volatile("movq %%fs, %0\n"
                 : "=r"(ret));
    return ret;
}

static CPUData *GetCPU(uint64_t id) { return &CPUs[id]; }

#define CurrentCPU GetCurrentCPU()
