#include "kernel.h"

#include <internal_task.h>
#include <filesystem.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <test.h>
#include <asm.h>
#include <sys.h>
#include <io.h>

#include "drivers/keyboard.hpp"
#include "drivers/mouse.hpp"
#include "interrupts/pic.h"
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

GlobalBootParams earlyparams;
GlobalBootParams *bootparams = nullptr;
uint8_t kernel_stack[STACK_SIZE];

void KernelInit();

EXTERNC void stivale2_initializator(stivale2_struct *bootloaderdata)
{
    init_stivale2(bootloaderdata, &earlyparams, false);
    init_pmm();
    init_vmm();
    init_kernelpml();
    init_heap(AllocationAlgorithm::LibAlloc11);
    bootparams = new GlobalBootParams;
    debug("bootparams is allocated at %p", bootparams);
    debug("bootparams framebuffer is allocated at %p", bootparams->Framebuffer);
    debug("bootparams rsdp is allocated at %p", bootparams->rsdp);
    init_stivale2(bootloaderdata, bootparams, true);
    KernelInit();
}

EXTERNC void kernel_entry(void *data)
{
    err("Bootloader initialized the kernel with unknown protocol!");
    CPU_HALT;
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
    printf("%s", cpu_get_info());
#endif
    SysCreateProcessFromFile("/system/test1", 0, 0, User);
    SysCreateProcessFromFile("/system/test2", 0, 0, User);
    SysCreateProcessFromFile("/system/test3", 0, 0, User);
    SysCreateProcessFromFile("/system/test4", 0, 0, User);
    trace("End Of Kernel Task");
    CPU_STOP;
}

void KernelInit()
{
    trace("Early initialization completed.");
    TEST_TEST();
    do_libs_test();
    SymTbl = new KernelSymbols::Symbols;
    CurrentDisplay = new DisplayDriver::Display;
    CurrentDisplay->Clear();
    printf("- KERNEL  %s\n", KERNEL_NAME);
    printf("- VERSION %s\n", KERNEL_VERSION);
    printf("- GIT     %s\n", GIT_COMMIT);
    printf("Kernel is starting up");
    KernelPageTableAllocator = new PageTableHeap::PageTableHeap;
    KernelStackAllocator = new StackHeap::StackHeap;
    CurrentDisplay->KernelPrint('.');
    init_gdt();
    init_idt();
    init_tss();
    CurrentDisplay->KernelPrint('.');
    acpi = new ACPI::ACPI;
    madt = new ACPI::MADT;
    init_pci();
    apic = new APIC::APIC;
    CurrentDisplay->KernelPrint('.');
    smp = new SymmetricMultiprocessing::SMP;
    apic->RedirectIRQs();
    init_timer();
    do_stacktrace_test();
    CurrentDisplay->KernelPrint('.');
    if (!apic->APICSupported())
    {
        panic("APIC is not supported!", true);
        outb(PIC1_DATA, 0b11111000);
        outb(PIC2_DATA, 0b11101111);
    }
    STI;
    ps2keyboard = new PS2Keyboard::PS2KeyboardDriver;
    vfs = new FileSystem::Virtual;
    CurrentDisplay->KernelPrint('.');
    for (int i = 0; i < bootparams->modules.num; i++)
        if (bootparams->modules.ramdisks[i].type == initrdType::USTAR)
        {
            new FileSystem::USTAR(bootparams->modules.ramdisks[i].start);
            break;
        }
    devfs = new FileSystem::Device;
    mountfs = new FileSystem::Mount;
    procfs = new FileSystem::Process;
    CurrentDisplay->KernelPrint('.');
    new FileSystem::Serial;
    new FileSystem::Random;
    new FileSystem::Null;
    new FileSystem::Zero;
    ps2mouse = new PS2Mouse::PS2MouseDriver;
    CurrentDisplay->KernelPrint('.');
    printf("\nKERNEL EARLY INITIALIZATION COMPLETE\nRUNNING KERNEL TASK...\n");
    StartTasking((uint64_t)KernelTask);
    CPU_STOP;
}
