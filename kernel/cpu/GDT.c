#include "gdt.h"
#include "../kernel.h"
#include <heap.h>
#include <lock.h>
#include <asm.h>

static GlobalDescriptorTableEntries GDT_Entries = {
    {.Length = 0x0000, .BaseLow = 0x0000, .BaseMiddle = 0x00, .Access = 0b00000000, .Flags = 0b00000000, .BaseHigh = 0x00},    // null
    {.Length = 0x0000, .BaseLow = 0x0000, .BaseMiddle = 0x00, .Access = 0b10011010, .Flags = 0b00100000, .BaseHigh = 0x00},    // kernel code
    {.Length = 0x0000, .BaseLow = 0x0000, .BaseMiddle = 0x00, .Access = 0b10010010, .Flags = 0b00000000, .BaseHigh = 0x00},    // kernel data
    {.Length = 0x0000, .BaseLow = 0x0000, .BaseMiddle = 0x00, .Access = 0b11111010, .Flags = 0b00100000, .BaseHigh = 0x00},    // user code
    {.Length = 0x0000, .BaseLow = 0x0000, .BaseMiddle = 0x00, .Access = 0b11110010, .Flags = 0b00000000, .BaseHigh = 0x00},    // user data
    {.Length = 0, .Low = 0, .Middle = 0, .High = 0, .Upper32 = 0, .Flags1 = 0b10001001, .Flags2 = 0b00000000, .Reserved = 0}}; // tss

GlobalDescriptorTableDescriptor gdt = {.Length = sizeof(GlobalDescriptorTableEntries) - 1, .Entries = &GDT_Entries};

void init_gdt()
{
    trace("Setting up GDT...");
    lgdt(gdt);

    asm volatile("movq %%rsp, %%rax\n"
                 "pushq $16\n"
                 "pushq %%rax\n"
                 "pushfq\n"
                 "pushq $8\n"
                 "pushq $1f\n"
                 "iretq\n"
                 "1:\n"
                 "movw $16, %%ax\n"
                 "movw %%ax, %%ds\n"
                 "movw %%ax, %%es\n"
                 "movw $0x1b, %%ax\n"
                 "movw %%ax, %%fs\n"
                 "movw %%ax, %%gs\n" ::
                     : "memory", "rax");
    debug("KC:%#llx|KD:%#llx|UD:%#llx|UC:%#llx|TSS:%#llx", GDT_KERNEL_CODE, GDT_KERNEL_DATA, GDT_USER_DATA, GDT_USER_CODE, GDT_TSS);
    debug("KC:%d|KD:%d|UD:%d|UC:%d|TSS:%d", GDT_KERNEL_CODE, GDT_KERNEL_DATA, GDT_USER_DATA, GDT_USER_CODE, GDT_TSS);
}

TaskStateSegment *tss = NULL;

void init_tss()
{
    trace("initializing tss");
    tss = (TaskStateSegment *)kcalloc(bootparams->smp.CPUCount, sizeof(TaskStateSegment));
    uint64_t tss_base = (uint64_t)&tss[0];
    gdt.Entries->TaskStateSegment.Length = tss_base + sizeof(tss[0]);
    gdt.Entries->TaskStateSegment.Low = (uint16_t)(tss_base & 0xFFFF);
    gdt.Entries->TaskStateSegment.Middle = (uint8_t)((tss_base >> 16) & 0xFF);
    gdt.Entries->TaskStateSegment.High = (uint8_t)((tss_base >> 24) & 0xFF);
    gdt.Entries->TaskStateSegment.Upper32 = (uint32_t)((tss_base >> 32) & 0xFFFFFFFF);
    gdt.Entries->TaskStateSegment.Flags1 = 0b10001001;
    gdt.Entries->TaskStateSegment.Flags2 = 0b00000000;
    (&tss[0])->IOMapBaseAddressOffset = sizeof(TaskStateSegment);
    (&tss[0])->StackPointer[0] = (uint64_t)RequestPage();
    (&tss[0])->InterruptStackTable[0] = (uint64_t)RequestPage(); // exceptions
    (&tss[0])->InterruptStackTable[1] = (uint64_t)RequestPage(); // nmi
    (&tss[0])->InterruptStackTable[2] = (uint64_t)RequestPage(); // page fault, double fault, general protection fault, etc...

    ltr(GDT_TSS);
    asm volatile("mov %%rsp, %0"
                 : "=r"((&tss[0])->StackPointer[0]));
}

NEWLOCK(tss_lock);

void CreateNewTSS(int CPUCore)
{
    LOCK(tss_lock);
    trace("initializing task state segment for CPU %d", CPUCore);
    tss = (TaskStateSegment *)kmalloc(sizeof(TaskStateSegment));
    uint64_t tss_base = (uint64_t)&tss[CPUCore];
    gdt.Entries->TaskStateSegment.Length = tss_base + sizeof(tss[CPUCore]);
    gdt.Entries->TaskStateSegment.Low = (uint16_t)(tss_base & 0xFFFF);
    gdt.Entries->TaskStateSegment.Middle = (uint8_t)((tss_base >> 16) & 0xFF);
    gdt.Entries->TaskStateSegment.High = (uint8_t)((tss_base >> 24) & 0xFF);
    gdt.Entries->TaskStateSegment.Upper32 = (uint32_t)((tss_base >> 32) & 0xFFFFFFFF);
    gdt.Entries->TaskStateSegment.Flags1 = 0b10001001;
    gdt.Entries->TaskStateSegment.Flags2 = 0b00000000;
    (&tss[CPUCore])->IOMapBaseAddressOffset = sizeof(TaskStateSegment);
    ltr(GDT_TSS);
    (&tss[CPUCore])->StackPointer[0] = (uint64_t)kernel_stack;
    (&tss[CPUCore])->InterruptStackTable[0] = (uint64_t)RequestPage(); // exceptions
    (&tss[CPUCore])->InterruptStackTable[1] = (uint64_t)RequestPage(); // nmi
    (&tss[CPUCore])->InterruptStackTable[2] = (uint64_t)RequestPage(); // page fault, double fault, general protection fault, etc...
    UNLOCK(tss_lock);
}
