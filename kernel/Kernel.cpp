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
    CPUs[0].Checksum = CPU_DATA_CHECKSUM;
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

void drawrectangle(uint64_t X, uint64_t Y, uint64_t W, uint64_t H, uint32_t C)
{
    uint64_t address = CurrentDisplay->GetFramebuffer()->Address;
    uint64_t width = CurrentDisplay->GetFramebuffer()->Width;
    uint64_t height = CurrentDisplay->GetFramebuffer()->Height;
    uint64_t ppsl = CurrentDisplay->GetFramebuffer()->PixelsPerScanLine;
    for (int y = Y; y < Y + H; y++)
        for (int x = X; x < X + W; x++)
        {
            if (x >= width || y >= height)
                continue;
            ((uint32_t *)address)[x + (y * ppsl)] = C;
        }
}

void testkapp1()
{
    while (1)
        drawrectangle(0, 0, 20, 100, 0xFFFFFF);
    return;
}

void testkapp2()
{
    while (1)
        drawrectangle(20, 0, 20, 100, 0xFFFF00);
    return;
}

void testkapp3()
{
    while (1)
        drawrectangle(40, 0, 20, 100, 0xFF00FF);
    return;
}

void testkapp4()
{
    while (1)
        drawrectangle(60, 0, 20, 100, 0x00FFFF);
    return;
}

void testkapp5()
{
    while (1)
        drawrectangle(80, 0, 20, 100, 0x00FF00);
    return;
}

void testkapp6()
{
    while (1)
        drawrectangle(100, 0, 20, 100, 0xFF0000);
    return;
}

void testkapp21()
{
    while (1)
        drawrectangle(0, 100, 20, 100, 0xFFFFFF);
    return;
}

void testkapp22()
{
    while (1)
        drawrectangle(20, 100, 20, 100, 0xFFFF00);
    return;
}

void testkapp23()
{
    while (1)
        drawrectangle(40, 100, 20, 100, 0xFF00FF);
    return;
}

void testkapp24()
{
    while (1)
        drawrectangle(60, 100, 20, 100, 0x00FFFF);
    return;
}

void testkapp25()
{
    while (1)
        drawrectangle(80, 100, 20, 100, 0x00FF00);
    return;
}

void testkapp26()
{
    while (1)
        drawrectangle(100, 100, 20, 100, 0xFF0000);
    return;
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

    PCB *kapp = SysCreateProcess("KernelTaskingTest1", Kernel);
    SysCreateThread(kapp, (uint64_t)testkapp1, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp2, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp3, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp4, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp5, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp6, 0, 0);

    PCB *kapp100 = SysCreateProcess("KernelTaskingTest2", Kernel);
    SysCreateThread(kapp100, (uint64_t)testkapp21, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp22, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp23, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp24, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp25, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp26, 0, 0);

    // SysCreateProcessFromFile("/system/test1", 0, 0, User);
    // SysCreateProcessFromFile("/system/test2", 0, 0, User);
    // SysCreateProcessFromFile("/system/test3", 0, 0, User);
    // SysCreateProcessFromFile("/system/test4", 0, 0, User);
    trace("End Of Kernel Task");
    while (1)
        drawrectangle(0, 0, 120, 200, 0x000000);
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
