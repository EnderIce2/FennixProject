#include "kernel.h"

#include <internal_task.h>
#include <filesystem.h>
#include <bootscreen.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <test.h>
#include <asm.h>
#include <sys.h>
#include <io.h>
#include <vm.h>

#include "sysrecovery/recovery.hpp"
#include "drivers/keyboard.hpp"
#include "drivers/mouse.hpp"
#include "interrupts/pic.h"
#include "drivers/disk.h"
#include "cpu/acpi.hpp"
#include "cpu/apic.hpp"
#include "cpu/smp.hpp"
#include "cpu/cpuid.h"
#include "driver.hpp"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "timer.h"
#include "test.h"
#include "pci.h"

// Early params are used only for the most basic functions. After that it may be overwritten with other data.
GlobalBootParams earlyparams;
GlobalBootParams *bootparams = nullptr;
SysFlags *sysflags = nullptr;
uint8_t kernel_stack[STACK_SIZE];

void KernelInit();

EXTERNC void stivale2_initializator(stivale2_struct *bootloaderdata)
{
    init_stivale2(bootloaderdata, &earlyparams, false);
    // asm volatile("andq $-16, %rsp");
    // asm volatile("movw $0, %ax");
    // asm volatile("movw %ax, %fs");
    // wrmsr(MSR_FS_BASE, 0);

    init_pmm();
    init_vmm();
    init_kernelpml();
    init_heap(AllocationAlgorithm::XallocV1);
    bootparams = new GlobalBootParams;
    debug("bootparams is allocated at %p", bootparams);
    debug("bootparams framebuffer is allocated at %p", bootparams->Framebuffer);
    debug("bootparams rsdp is allocated at %p", bootparams->rsdp);
    sysflags = new SysFlags;
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
    else
        err("No bootloader protocol found. System Halted.");
    CPU_HALT;
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

bool ShowRecoveryScreen = false;

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

    // for now i'll not load this.
    // if (CurrentTaskingMode != TaskingMode::Mono)
    if (0 != 0)
    {
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
                            CurrentDisplay->SetPrintColor(0xCCCCCC);
                            printf("Loading driver %s... ", driver->Name);
                            uint64_t ret = kdrv->LoadKernelDriverFromFile(driver);
                            if (ret == 0)
                            {
                                CurrentDisplay->SetPrintColor(0x058C19);
                                printf("OK\n");
                            }
                            else
                            {
                                CurrentDisplay->SetPrintColor(0xE85230);
                                printf("FAILED (%#lx)\n", ret);
                            }
                            CurrentDisplay->ResetPrintColor();
                            // TODO: get instruction pointer of the elf entry point.
                            BS->IncreaseProgres();
                        }
                    }
            }
        }
        vfs->Close(driverDirectory);
    }

    BS->Progress(100);

    if (ShowRecoveryScreen)
        new SystemRecovery::Recovery;

    if (!SysCreateProcessFromFile("/system/init", 0, 0, User))
    {
        CurrentDisplay->SetPrintColor(0xFC4444);
        printf("Failed to load /system/init process. The file is missing or corrupted.\n");
        CPU_STOP;
    }
    trace("End Of Kernel Task");
    CPU_STOP;
}

/* I should make everything in C++ but I use code from older (failed) projects.
   I will probably move the old C code to C++ in the future. */

void KernelInit()
{
    trace("Early initialization completed.");
    TEST_TEST();
    do_libs_test();
    do_mem_test();
    initializeKernelFlags();
    BS = new BootScreen::Screen;
    SymTbl = new KernelSymbols::Symbols;
    BS->IncreaseProgres();
    CurrentDisplay = new DisplayDriver::Display;
    KernelPageTableAllocator = new PageTableHeap::PageTableHeap;
    KernelStackAllocator = new StackHeap::StackHeap;
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
    init_timer();
    // TODO: after APIC timer is initialized the memory gets corrupted, or there is another function that breaks the memory
    do_interrupts_mem_test();
    BS->IncreaseProgres();
    apic->RedirectIRQs();

    do_stacktrace_test();

    if (!apic->APICSupported())
    {
        panic("APIC is not supported!", true);
        outb(PIC1_DATA, 0b11111000);
        outb(PIC2_DATA, 0b11101111);
    }
    STI;
    dsdt->InitSCI();

    ps2keyboard = new PS2Keyboard::PS2KeyboardDriver;
    BS->IncreaseProgres();

    if (!strstr(bootparams->cmdline, "novmwarn"))
        if (!CheckRunningUnderVM())
        {
            CurrentDisplay->SetPrintColor(0xFC4444);
            printf("WARNING!\nThe kernel has detected that you are booting from a real computer!\nBeaware that this project is not in a stable state and will likely cause problems like, overwriting data on disks or even worse, breaking the entire system!\nIf you REALLY want to continue, write \"YES\" and press enter.\nIf you want to disable this warning add \"novmwarn\" in kernel's cmdline.\n\n> ");
            const char scancodes[] = {'?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};
            char buffer[16];
            bool done = false;
            while (1)
            {
                if (done)
                    break;
                uint8_t c = ps2keyboard->GetLastScanCode();
                if (!(c & 0x80))
                {
                    switch (c)
                    {
                    case 0x15: // y
                        CurrentDisplay->SetPrintColor(0x00FC00);
                        break;
                    case 0x12: // e
                        CurrentDisplay->SetPrintColor(0x00FC00);
                        break;
                    case 0x1f: // s
                        CurrentDisplay->SetPrintColor(0x00FC00);
                        break;
                    case 0x1c: // enter
                        done = true;
                        [[fallthrough]];
                    default:
                        CurrentDisplay->SetPrintColor(0xFC4444);
                        break;
                    }
                    if (c != 0x1c)
                    {
                        int len = strlen(buffer);
                        if (len > 16)
                            break;
                        buffer[len] = scancodes[c];
                        buffer[len + 1] = '\0';
                        printf("%c", buffer[len]);
                    }
                }
            }
            if (strcmp("YES", buffer))
            {
                CurrentDisplay->SetPrintColor(0xFC4444);
                printf("\n\nWrong word! Shutting down...");
                sleep(5);
                dsdt->shutdown();
                CPU_STOP;
            }
            CurrentDisplay->ResetPrintColor();
        }

    vfs = new FileSystem::Virtual;
    BS->Progress(50);

    for (int i = 0; i < bootparams->modules.num; i++)
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

    ps2mouse = new PS2Mouse::PS2MouseDriver;
    BS->IncreaseProgres();

    do_mem_test();
    do_tasking_test();

    // #ifndef TESTING
    if (sysflags->monotasking)
        StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    else
        StartTasking((uint64_t)KernelTask, TaskingMode::Multi);
    // #endif
    CPU_STOP;
}
