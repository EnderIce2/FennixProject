#include "kernel.h"

#include <filesystem.h>
#include <bootscreen.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <task.h>
#include <io.h>
#include <vm.h>

#include "drivers/disk.h"
#include "cpu/cpuid.h"
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
    init_heap(AllocationAlgorithm::Default);
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

#ifdef DEBUG
inline void FileListColorHelper(uint64_t type)
{
    switch (type)
    {
    case FileSystem::NodeFlags::FS_ERROR:
        CurrentDisplay->SetPrintColor(0xFF0000);
        break;
    case FileSystem::NodeFlags::FS_FILE:
        CurrentDisplay->SetPrintColor(0x01f395);
        break;
    case FileSystem::NodeFlags::FS_DIRECTORY:
        CurrentDisplay->SetPrintColor(0x29a31b);
        break;
    case FileSystem::NodeFlags::FS_CHARDEVICE:
        CurrentDisplay->SetPrintColor(0xa39a1b);
        break;
    case FileSystem::NodeFlags::FS_BLOCKDEVICE:
        CurrentDisplay->SetPrintColor(0x6da31b);
        break;
    case FileSystem::NodeFlags::FS_MOUNTPOINT:
        CurrentDisplay->SetPrintColor(0xa3731b);
        break;
    default:
        CurrentDisplay->SetPrintColor(0xFFFFFF);
        break;
    }
}
#endif

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
    if (CheckRunningUnderVM())
    {
        printf("Running under VM.\n");
    }
    else
    {
        printf("Running under physical hardware.\n");
    }
    FileSystem::FILE *file = vfs->Open("/");
    if (file->Status == FileSystem::FILESTATUS::OK)
    {
        foreach (auto var in file->Node->Children)
        {
            FileListColorHelper(var->Flags);
            printf("\n/%s", var->Name);
            foreach (auto var in var->Children)
            {
                FileListColorHelper(var->Flags);
                printf("\n  %s", var->Name);
                foreach (auto var in var->Children)
                {
                    FileListColorHelper(var->Flags);
                    printf("\n   %s", var->Name);
                    foreach (auto var in var->Children)
                    {
                        FileListColorHelper(var->Flags);
                        printf("\n    %s", var->Name);
                        foreach (auto var in var->Children)
                        {
                            FileListColorHelper(var->Flags);
                            printf("\n     %s", var->Name);
                            foreach (auto var in var->Children)
                            {
                                FileListColorHelper(var->Flags);
                                printf("\n      %s", var->Name);
                            }
                        }
                    }
                }
            }
        }
    }
    else
        printf("FileSystem error: %d\n", file->Status);
    printf("\n");
    FileListColorHelper(-1);
    vfs->Close(file);
    printf("%s", cpu_get_info());
#endif

    FileSystem::FILE *driverDirectory = vfs->Open("/system/drivers");
    if (driverDirectory->Status == FileSystem::FILESTATUS::OK)
    {
        foreach (auto driver in driverDirectory->Node->Children)
        {
            if (cwk_path_has_extension(driver->Name))
            {
                const char *extension;
                cwk_path_get_extension(driver->Name, &extension, nullptr);

                if (!strcmp(extension, ".drv"))
                {
                    printf("Loading driver %s...\n", driver->Name);
                    // TODO: get instruction pointer of the elf entry point.
                    BS->IncreaseProgres();
                }
            }
        }
    }
    vfs->Close(driverDirectory);

    BS->Progress(100);
    if (!SysCreateProcessFromFile("/system/init", true))
    {
        CurrentDisplay->SetPrintColor(0xFFFC4444);
        printf("Failed to load /system/init process. The file is missing or corrupted.\n");
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

    if (!strstr(bootparams->cmdline, "novmwarn"))
        if (!CheckRunningUnderVM())
        {
            // TODO: Warn for possible hardware damage.
            CurrentDisplay->SetPrintColor(0xFFFC4444);
            printf("WARNING!\nThe kernel has detected that you are booting from a real computer!\nBeaware that this project is not in a stable state and will likely cause problems like, overwriting data on disks or even worse, breaking the entire system!\nIf you REALLY want to continue, write \"YES\" and press enter.\nIf you want to disable this warning add \"novmwarn\" in kernel's cmdline.\n");
            CurrentDisplay->ResetPrintColor();
        }

    vfs = new FileSystem::Virtual();

    BS->Progress(50);
    // Make sure that after vfs initialization we set the root "/" directory.
    // TODO: we should check if it's installed in a system disk instead of initrd. this is a must because we need to set first the root and then mount everything else.
    for (size_t i = 0; i < bootparams->modules.num; i++)
    {
        BS->IncreaseProgres();
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

    devfs = new FileSystem::Device();
    mountfs = new FileSystem::Mount();
    diskmgr = new DiskManager::Disk();
    partmgr = new DiskManager::Partition();
    BS->Progress(70);

    new FileSystem::Serial;
    new FileSystem::Random;
    new FileSystem::Null;
    new FileSystem::Zero;
    /* ... */

    // StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    StartTasking((uint64_t)KernelTask, TaskingMode::Multi);

    err("Unwanted reach.");
    CPU_STOP;
}
