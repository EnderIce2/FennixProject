#include "kernel.h"

#ifdef __linux__
#error "Cross-compiler not detected."
#endif

#include <internal_task.h>
#include <filesystem.h>
#include <bootscreen.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <ipc.hpp>
#include <test.h>
#include <asm.h>
#include <sys.h>
#include <io.h>
#include <vm.h>

#include "network/NetworkController.hpp"
#include "sysrecovery/recovery.hpp"
#include "drivers/keyboard.hpp"
#include "drivers/mouse.hpp"
#include "interrupts/pic.h"
#include "drivers/disk.h"
#include "cpu/acpi.hpp"
#include "cpu/apic.hpp"
#include "proc/ipc.hpp"
#include "cpu/smp.hpp"
#include "cpu/cpuid.h"
#include "driver.hpp"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "timer.h"
#include "test.h"
#include "pci.h"

GlobalBootParams earlyparams;
GlobalBootParams *bootparams = nullptr;
SysFlags *sysflags = nullptr;
uint8_t kernel_stack[STACK_SIZE];

bool ShowRecoveryScreen = false;
bool FadeScreenNow = false;
bool KernelTaskStarted = false;

Xalloc::AllocatorV1 *UserAllocator = nullptr; // TODO: Fix this allocator or modify liballoc11

void KernelInit();

EXTERNC void limine_initializator()
{
    if (!init_limine(&earlyparams, false))
    {
        err("Failed to initialize limine");
        CPU_HALT;
    }

#if defined(__amd64__) || defined(__i386__)
    wrmsr(MSR_FS_BASE, 0);
#endif
    init_pmm();
    init_vmm();
    init_kernelpml();
    // init_heap(AllocationAlgorithm::XallocV1);
    init_heap(AllocationAlgorithm::LibAlloc11);
    bootparams = new GlobalBootParams;
    debug("bootparams is allocated at %p", bootparams);
    debug("bootparams framebuffer is allocated at %p", bootparams->Framebuffer);
    debug("bootparams rsdp is allocated at %p", bootparams->rsdp);
    sysflags = new SysFlags;

    init_limine(bootparams, true);
    KernelInit();
}

EXTERNC void multiboot2_initializator()
{
    err("stub");
    CPU_HALT;
}

EXTERNC void kernel_entry(void *Data)
{
    err("Bootloader initialized the kernel with unknown protocol!");
    err("System Halted.");
    CPU_HALT;
}

EXTERNC void kernel_main_aarch64()
{
}

void initializeKernelFlags()
{
    if (strstr(bootparams->cmdline, "debug"))
        sysflags->fennecsarethebest = true;
    else
        sysflags->fennecsarethebest = false;
    if (strstr(bootparams->cmdline, "rootfs")) // FIXME
        sysflags->rootfs[0] = '/';
    else
        sysflags->rootfs[0] = '/';
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
    if (strstr(bootparams->cmdline, "monotasking"))
        sysflags->monotasking = true;
    else
        sysflags->monotasking = false;
}

void KernelTask()
{
#if defined(__amd64__) || defined(__i386__)
    if (KernelTaskStarted)
        panic("Kernel Task restarted! System Halted!\n", false);

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

    if (CurrentTaskingMode != TaskingMode::Mono)
    {
        ipc = new InterProcessCommunication::IPC;
        kdrv = new Driver::KernelDriver;
    }

    nimgr->StartService();

    if (ShowRecoveryScreen)
        new SystemRecovery::Recovery;

    // if (!SysCreateProcessFromFile("/system/init", 0, 0, CBElevation::User))
    // {
    //     CurrentDisplay->SetPrintColor(0xFC4444);
    //     printf("Failed to load /system/init process. The file is missing or corrupted.\n");
    //     CPU_HALT;
    // }

    trace("End Of Kernel Task");
    if (CurrentTaskingMode != TaskingMode::Mono)
    {
        while (!FadeScreenNow)
            HLT;
        BS->FadeLogo();
        CPU_STOP;
    }
    else
        KernelTaskStarted = true;
#endif
}

void CheckSystemRequirements()
{
#ifndef DEBUG
    CurrentDisplay = new DisplayDriver::Display;
    if (!cpu_feature(CPUID_FEAT_RDX_TSC))
        panic("The current CPU doesn't support the minimum system requirements. (TSC)", true);
    if (!cpu_feature(CPUID_FEAT_RDX_SSE))
        panic("The current CPU doesn't support the minimum system requirements. (SSE)", true);
    if (!cpu_feature(CPUID_FEAT_RDX_UMIP))
        panic("The current CPU doesn't support the minimum system requirements. (UMIP)", true);
    if (!cpu_feature(CPUID_FEAT_RDX_SMEP))
        panic("The current CPU doesn't support the minimum system requirements. (SMEP)", true);
    if (!cpu_feature(CPUID_FEAT_RDX_SMAP))
        panic("The current CPU doesn't support the minimum system requirements. (SMAP)", true);
    if (!cpu_feature(CPUID_FEAT_RCX_RDRAND))
        panic("The current CPU doesn't support the minimum system requirements. (RDRAND)", true);
    delete CurrentDisplay;
#endif
}

void KernelInit()
{
    trace("Early initialization completed.");
    TEST_TEST();
    do_libs_test();
    do_mem_test();
    initializeKernelFlags();
    CheckSystemRequirements();
    BS = new BootScreen::Screen;
    SymTbl = new KernelSymbols::Symbols;
    BS->IncreaseProgres();
    CurrentDisplay = new DisplayDriver::Display;
    KernelPageTableAllocator = new PageTableHeap::PageTableHeap;
    KernelStackAllocator = new StackHeap::StackHeap;
#if defined(__amd64__) || defined(__i386__)
    init_gdt();
    BS->IncreaseProgres();
    init_idt();
    BS->IncreaseProgres();
    init_tss();
    BS->IncreaseProgres();
    acpi = new ACPI::ACPI;
    BS->IncreaseProgres();
    madt = new ACPI::MADT;
    BS->IncreaseProgres();
    dsdt = new ACPI::DSDT;
    BS->IncreaseProgres();
    init_pci();
    BS->Progress(40);
    apic = new APIC::APIC;
    BS->IncreaseProgres();
    smp = new SymmetricMultiprocessing::SMP;
    BS->IncreaseProgres();
    apic->RedirectIRQs();
    BS->IncreaseProgres();
    init_timer();
    // do_interrupts_mem_test();

    do_stacktrace_test();

    if (!apic->APICSupported())
    {
        panic("APIC is not supported!", true);
        outb(PIC1_DATA, 0b11111000);
        outb(PIC2_DATA, 0b11101111);
    }
    UserAllocator = new Xalloc::AllocatorV1((void *)USER_HEAP_BASE, true, cpu_feature(CPUID_FEAT_RDX_SMAP));
    STI;
#ifdef DEBUG
    printf("STI\n");
#endif
    dsdt->InitSCI();

    ps2keyboard = new PS2Keyboard::PS2KeyboardDriver;
    BS->IncreaseProgres();

    if (!strstr(bootparams->cmdline, "novmwarn"))
        if (!CheckRunningUnderVM())
        {
            CurrentDisplay->SetPrintColor(0xFC4444);
            printf("WARNING!\nThe kernel has detected that you are booting from a real computer!\n");
            printf("Beaware that this project is not in a stable state and will likely cause problems like, overwriting data on disks or even worse, breaking the entire system!\n");
            printf("If you REALLY want to continue, press ENTER.\n");
            CurrentDisplay->SetPrintColor(0x0C8464);
            printf("\nTo disable this warning add \"novmwarn\" in kernel's cmdline.\n");
            CurrentDisplay->ResetPrintColor();
            if (ps2keyboard->WaitScanCode() != 0x45)
            {
                CurrentDisplay->SetPrintColor(0xFC4444);
                printf("\n\nWrong key! Shutting down...");
                sleep(5);
                dsdt->shutdown();
                CPU_HALT;
            }
        }

    vfs = new FileSystem::Virtual;
    BS->Progress(50);

    new FileSystem::USTAR(bootparams->Modules.Module[0].Start);
    devfs = new FileSystem::Device;
    mountfs = new FileSystem::Mount;
    procfs = new FileSystem::Process;
    diskmgr = new DiskManager::Disk;
    partmgr = new DiskManager::Partition;
    sysfs = new FileSystem::SysInfo;
    BS->Progress(70);
    do_network_test();
    nimgr = new NetworkInterfaceManager::NetworkInterface;
    BS->Progress(75);

    new FileSystem::Serial;
    new FileSystem::Random;
    new FileSystem::Null;
    new FileSystem::Zero;
    FileSystem::FB *fb = new FileSystem::FB;
    fb->SetFrameBufferData(bootparams->Framebuffer.BaseAddress,
                           bootparams->Framebuffer.BufferSize,
                           bootparams->Framebuffer.Width,
                           bootparams->Framebuffer.Height,
                           bootparams->Framebuffer.PixelsPerScanLine);

    ps2mouse = new PS2Mouse::PS2MouseDriver;
    BS->IncreaseProgres();

    do_mem_test();
    // do_tasking_test();

    BS->Progress(100);

    if (sysflags->monotasking)
        StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    else
        StartTasking((uint64_t)KernelTask, TaskingMode::Multi);
#endif
    CPU_STOP;
}
