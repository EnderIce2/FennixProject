#include "kernel.h"

#include <filesystem.h>
#include <bootscreen.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <task.h>
#include <io.h>

#include "drivers/disk.h"
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

void KernelInit();

EXTERNC void stivale_initializator(stivale_struct *bootloaderdata)
{
    // TODO: support stivale boot protocol.
    init_stivale(bootloaderdata, &earlyparams);
    CPU_STOP;
    KernelInit();
}

EXTERNC void stivale2_initializator(stivale2_struct *bootloaderdata)
{
    init_stivale2(bootloaderdata, &earlyparams, false);
    init_pmm();
    init_vmm();
    init_kernelpml();
    init_heap(AllocationAlgorithm::LibAlloc11);
    bootparams = (GlobalBootParams *)kcalloc(1, sizeof(GlobalBootParams));
    bootparams->Framebuffer = (GBPFramebuffer *)kcalloc(1, sizeof(GBPFramebuffer));
    bootparams->rsdp = (GBPRSDP *)kcalloc(1, sizeof(GBPRSDP));
    sysflags = (SysFlags *)kcalloc(1, sizeof(SysFlags));
    sysflags->rootfs = (string)kcalloc(1, sizeof(string));
    init_stivale2(bootloaderdata, bootparams, true);

    KernelInit();
}

EXTERNC void limine_initializator()
{
    // TODO: support limine boot protocol.
    init_limine(&earlyparams);
    CPU_STOP;
    KernelInit();
}

EXTERNC void kernel_entry(void *data)
{
    // unknown boot protocol.
    err("Bootloader initialized the kernel with unknown protocol! Trying to figure out what protocol is used.");
    // TODO: detect boot protocol. make it more safe. it can trigger a triple fault.
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
    if (strstr(bootparams->cmdline, "debug"))
        sysflags->fennecsarethebest = true;
    else
        sysflags->fennecsarethebest = false;
    if (strstr(bootparams->cmdline, "rootfs")) // FIXME
        sysflags->rootfs = "/";
    else
        sysflags->rootfs = "/";
    if (strstr(bootparams->cmdline, "nogpu"))
        sysflags->nogpu = true;
    else
        sysflags->nogpu = false;
    if (strstr(bootparams->cmdline, "nohpet"))
        sysflags->nohpet = true;
    else
        sysflags->nohpet = false;
    if (strstr(bootparams->cmdline, "emergency"))
        sysflags->emergency = true;
    else
        sysflags->emergency = false;
    if (strstr(bootparams->cmdline, "nomount"))
        sysflags->nomount = true;
    else
        sysflags->nomount = false;
    if (strstr(bootparams->cmdline, "noloadingscreen"))
        sysflags->noloadingscreen = true;
    else
        sysflags->noloadingscreen = false;
}

void KernelTask()
{
#ifdef DEBUG
    debug("Hello World!");
    printf("This is a text to test if the OS is working properly.\n");
    CurrentDisplay->SetPrintColor(0xFFE85230);
    printf("The quick brown fox jumps over the lazy dog.\n");
    CurrentDisplay->SetPrintColor(0xFF1FF2EB);
    printf("1234567890-=!@#$%%^&*()_+[]\\;',./{}|:\"<>?\n");
    CurrentDisplay->ResetPrintColor();
    printf("Kernel Compiled at: %s %s with C++ Standard: %d\n", __DATE__, __TIME__, CPP_LANGUAGE_STANDARD);
    printf("C++ Language Version (__cplusplus) :%ld\n", __cplusplus);
#endif
    BS->Progress(100);
    if (!SysCreateProcessFromFile("/bin/finit", true))
    {
        CurrentDisplay->SetPrintColor(0xFFFC4444);
        printf("Failed to load /bin/finit process. The file is missing or corrupted.\n");
    }
    trace("End Of Kernel Task");
}

/* I should make everything in C++ but I use code from older (failed) projects.
   I will probably move the old C code to C++ in the future. */

void KernelInit()
{
    trace("early initialization completed");
    BS = new BootScreen::Screen();
    initflags();
    CurrentDisplay = new DisplayDriver::Display();
    KernelPageTableAllocator = new PageTableHeap::PageTableHeap();
    KernelStackAllocator = new StackHeap::StackHeap();
    init_gdt();
    BS->IncreaseProgres();
    init_idt();
    BS->IncreaseProgres();
    init_tss();
    BS->IncreaseProgres();
    enable_sse();
    BS->IncreaseProgres();
    SymTbl = new KernelSymbols::Symbols();
    init_acpi();
    init_pci();
    init_timer();
    BS->IncreaseProgres();
    BS->Progress(40);

    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);
    asm("sti");

    vfs = new FileSystem::Virtual();
    devfs = new FileSystem::Device();
    mountfs = new FileSystem::Mount();
    diskmgr = new DiskManager::Disk();
    partmgr = new DiskManager::Partition();
    BS->Progress(70);

    for (size_t i = 0; i < bootparams->modules.num; i++)
    {
        if (bootparams->modules.ramdisks[i].type == initrdType::USTAR)
        {
            new FileSystem::USTAR(bootparams->modules.ramdisks[i].start);
            break;
        }
        else if (bootparams->modules.ramdisks[i].type == initrdType::RAMFS)
        {
            new FileSystem::Initrd(bootparams->modules.ramdisks[i].start);
            break;
        }
    }

    new FileSystem::Serial;
    new FileSystem::Random;
    new FileSystem::Null;
    new FileSystem::Zero;
    /* ... */

    BS->Progress(90);
    // TODO: Page faults using multitasking mode. Is about switching tables? Somehow the PML4 is getting trashed? I don't really understand. But if we don't switch it, there are no issues.
    StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    // StartTasking((uint64_t)KernelTask, TaskingMode::Multi);

    err("Unwanted reach.");
    CPU_STOP;
}