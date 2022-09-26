#pragma once
#include <task.h>

#include "../kernel.h"
#include "acpi.hpp"
#include "idt.h"

#define CPU_DATA_CHECKSUM 0xdeadda7a

struct CPUData
{
    uint64_t ID;
    CR3 PageTable;
    GlobalDescriptorTableDescriptor GDT;
    InterruptDescriptorTableDescriptor IDT;
    TaskStateSegment TSS;

    PCB *CurrentProcess;
    TCB *CurrentThread;

    long ErrorCode;
    bool IsActive;
    long Checksum;

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

int GetCurrentCPUID();

CPUData *GetCurrentCPU();

CPUData *GetCPU(uint64_t id);

#define CurrentCPU GetCurrentCPU()
