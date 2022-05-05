#include "kernel.h"

#include <filesystem.h>
#include <bootscreen.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <internal_task.h>
#include <asm.h>
#include <io.h>
#include <vm.h>

#include "drivers/keyboard.hpp"
#include "drivers/mouse.hpp"
#include "drivers/disk.h"
#include "cpu/smp.hpp"
#include "cpu/cpuid.h"
#include "driver.hpp"
#include "cpu/gdt.h"
#include "cpu/idt.h"
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

void KernelTask()
{
#ifdef DEBUG
    debug("Hello World!");
    printf("This is a text to test if the OS is working properly.\n");
    uint32_t test = 0x00FF00;
    CurrentDisplay->SetPrintColor(test);
    for (size_t i = 0; i < 128; i++)
    {
        if (i != '\n')
            CurrentDisplay->KernelPrint(i);
        CurrentDisplay->SetPrintColor(test + i * 2);
    }
    CurrentDisplay->ResetPrintColor();
    CurrentDisplay->KernelPrint('\n');
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
    printf("%s", cpu_get_info());
#endif

    kdrv = new Driver::KernelDriver;
    FileSystem::FILE *driverDirectory = vfs->Open("/system/drivers");
    if (driverDirectory->Status == FileSystem::FILESTATUS::OK)
    {
        foreach (auto driver in driverDirectory->Node->Children)
        {
            if (driver->Flags == FileSystem::NodeFlags::FS_FILE)
                if (cwk_path_has_extension(driver->Name))
                {
                    const char *extension;
                    cwk_path_get_extension(driver->Name, &extension, nullptr);

                    if (!strcmp(extension, ".drv"))
                    {
                        CurrentDisplay->SetPrintColor(0xFFCCCCCC);
                        printf("Loading driver %s... ", driver->Name);
                        uint64_t ret = kdrv->LoadKernelDriverFromFile(driver);
                        if (ret == 0)
                        {
                            CurrentDisplay->SetPrintColor(0xFF058C19);
                            printf("OK\n");
                        }
                        else
                        {
                            CurrentDisplay->SetPrintColor(0xFFE85230);
                            printf("FAILED (%#llx)\n", ret);
                        }
                        CurrentDisplay->ResetPrintColor();
                        // TODO: get instruction pointer of the elf entry point.
                        BS->IncreaseProgres();
                    }
                }
        }
    }
    vfs->Close(driverDirectory);

    BS->Progress(100);
    if (!SysCreateProcessFromFile("/system/init", 0, 0, true))
    {
        CurrentDisplay->SetPrintColor(0xFFFC4444);
        printf("Failed to load /system/init process. The file is missing or corrupted.\n");
    }
    trace("End Of Kernel Task");
}

/* I should make everything in C++ but I use code from older (failed) projects.
   I will probably move the old C code to C++ in the future. */

void EnableCPUFeatures()
{
    CR0 cr0 = readcr0();
    CR4 cr4 = readcr4();
    if (cpu_feature(CPUID_FEAT_RDX_SSE))
    {
        debug("Enabling SSE support...");
        cr0.EM = 0;
        cr0.MP = 1;
        cr4.OSFXSR = 1;
        cr4.OSXMMEXCPT = 1;
    }
    // Not really useful at the moment and is causing a general protection fault (maybe because i don't know how to use it properly).
    // debug("Checking for UMIP, SMEP & SMAP support...");
    // if (cpu_feature(CPUID_FEAT_RDX_UMIP))
    //     cr4.UMIP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMEP))
    //     cr4.SMEP = 1;
    // if (cpu_feature(CPUID_FEAT_RDX_SMAP))
    //     cr4.SMAP = 1;
    writecr0(cr0);
    writecr4(cr4);
    debug("Enabling PAT support...");
    wrmsr(MSR_CR_PAT, 0x6 | (0x0 << 8) | (0x1 << 16));
    BS->IncreaseProgres();
}

void KernelInit()
{
    trace("early initialization completed");
    BS = new BootScreen::Screen;
    initflags();
    CurrentDisplay = new DisplayDriver::Display;
    KernelPageTableAllocator = new PageTableHeap::PageTableHeap;
    KernelStackAllocator = new StackHeap::StackHeap;
    init_gdt();
    BS->IncreaseProgres();
    init_idt();
    BS->IncreaseProgres();
    init_tss();
    BS->IncreaseProgres();
    SymTbl = new KernelSymbols::Symbols;
    init_acpi();
    init_pci();
    init_timer();
    smp = new SymmetricMultiprocessing::SMP;
    BS->Progress(40);
    EnableCPUFeatures();

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

    vfs = new FileSystem::Virtual;

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

    devfs = new FileSystem::Device;
    mountfs = new FileSystem::Mount;
    diskmgr = new DiskManager::Disk;
    partmgr = new DiskManager::Partition;
    BS->Progress(70);

    new FileSystem::Serial;
    new FileSystem::Random;
    new FileSystem::Null;
    new FileSystem::Zero;

    ps2keyboard = new PS2Keyboard::PS2KeyboardDriver;
    BS->IncreaseProgres();
    ps2mouse = new PS2Mouse::PS2MouseDriver;
    BS->IncreaseProgres();

    // StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    StartTasking((uint64_t)KernelTask, TaskingMode::Multi);
    // StartTasking((uint64_t)KernelTask, TaskingMode::MultiV2);
    CPU_STOP;
}
