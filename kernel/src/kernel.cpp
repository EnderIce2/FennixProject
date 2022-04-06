#include "kernel.h"

#include <display.h>
#include <string.h>

#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/sse.h"
#include "timer.h"
#include "acpi.h"
#include "pci.h"

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

void initflags()
{
    if (!strstr(bootparams->cmdline, "debug"))
        sysflags->fennecsarethebest = true;
    else
        sysflags->fennecsarethebest = false;
    if (!strstr(bootparams->cmdline, "rootfs")) // FIXME
        sysflags->rootfs = "/";
    else
        sysflags->rootfs = "/";
    if (!strstr(bootparams->cmdline, "nogpu"))
        sysflags->nogpu = true;
    else
        sysflags->nogpu = false;
    if (!strstr(bootparams->cmdline, "nohpet"))
        sysflags->nohpet = true;
    else
        sysflags->nohpet = false;
    if (!strstr(bootparams->cmdline, "emergency"))
        sysflags->emergency = true;
    else
        sysflags->emergency = false;
    if (!strstr(bootparams->cmdline, "nomount"))
        sysflags->nomount = true;
    else
        sysflags->nomount = false;

}

void merged_init()
{
    initflags();
    CurrentDisplay = DisplayDriver::Display();
    init_stack();
    init_gdt();
    init_idt();
    init_tss();
    enable_sse();
    init_timer();
    init_acpi();
    init_pci();
    debug("Hello World!");
    printf("This is a text to test if the OS is working properly.\n");
    CurrentDisplay.SetPrintColor(0xFFE85230);
    printf("The quick brown fox jumps over the lazy dog.\n");
    CurrentDisplay.SetPrintColor(0xFF1FF2EB);
    printf("1234567890-=!@#$%%^&*()_+[]\\;',./{}|:\"<>?\n");
    CPU_STOP;
}
