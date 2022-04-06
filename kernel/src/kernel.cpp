#include "kernel.h"
#include <display.h>
#include "cpu/gdt.h"
#include "cpu/idt.h"

// Early params are used only for the most basic functions. After that it may be overwritten with other data.
GlobalBootParams earlyparams;
GlobalBootParams *bootparams = nullptr;
SysFlags *sysflags = nullptr;
uint8_t kernel_stack[STACK_SIZE];

void merged_init();

EXTERNC void stivale_initializator(stivale_struct *bootloaderdata)
{
    // TODO: support stivale boot protocol.
    init_stivale(bootloaderdata, &earlyparams);
    CPU_STOP;
    merged_init();
}

EXTERNC void stivale2_initializator(stivale2_struct *bootloaderdata)
{
    init_stivale2(bootloaderdata, &earlyparams);
    init_pmm();
    init_vmm();
    init_heap((void *)KERNEL_HEAP_BASE, 0x10);
    bootparams = (GlobalBootParams *)kcalloc(1, sizeof(GlobalBootParams));
    bootparams->Framebuffer = (GBPFramebuffer *)kcalloc(1, sizeof(GBPFramebuffer));
    bootparams->rsdp = (GBPRSDP *)kcalloc(1, sizeof(GBPRSDP));
    sysflags = (SysFlags *)kcalloc(1, sizeof(SysFlags));
    sysflags->rootfs = (string)kcalloc(1, sizeof(string));
    init_stivale2(bootloaderdata, bootparams);

    merged_init();
}

EXTERNC void limine_initializator()
{
    // TODO: support limine boot protocol.
    init_limine(&earlyparams);
    CPU_STOP;
    merged_init();
}

EXTERNC void kernel_entry(void *data)
{
    // unknown boot protocol.
    err("Bootloader initialized the kernel with unknown protocol! Trying to figure out what protocol is used.");
    // TODO: detect boot protocol.
    if (detect_limine())
        limine_initializator();
    else if (detect_stivale2(data))
        stivale2_initializator(static_cast<stivale2_struct *>(data));
    else if (detect_stivale(data))
        stivale_initializator(static_cast<stivale_struct *>(data));
    else
        err("No bootloader protocol found. System Halted.");
    CPU_STOP;
}

void merged_init()
{
    CurrentDisplay = DisplayDriver::Display();
    init_gdt();
    init_idt();
    init_tss();
    debug("Hello World!");
    CPU_STOP;
}