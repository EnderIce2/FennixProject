#include "kernel.h"

#include <internal_task.h>
#include <filesystem.h>
#include <critical.hpp>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <cwalk.h>
#include <lock.h>
#include <asm.h>
#include <sys.h>
#include <io.h>

#include "drivers/keyboard.hpp"
#include "interrupts/pic.h"
#include "cpu/acpi.hpp"
#include "cpu/apic.hpp"
#include "cpu/smp.hpp"
#include "cpu/cpuid.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "timer.h"
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

uint64_t address = 0;
uint64_t width = 0;
uint64_t height = 0;
uint64_t ppsl = 0;

inline void drawrectangle(uint64_t X, uint64_t Y, uint64_t W, uint64_t H, uint32_t C)
{
    for (int y = Y; y < Y + H; y++)
        for (int x = X; x < X + W; x++)
        {
            if (x >= width || y >= height)
                continue;
            ((uint32_t *)address)[x + (y * ppsl)] = C;
        }
}

uint64_t bh = 0;

void testkapp1()
{
    while (1)
        drawrectangle(0, bh, 20, 100, 0xFFFFFF);
    return;
}

void testkapp2()
{
    while (1)
        drawrectangle(20, bh, 20, 100, 0xFFFF00);
    return;
}

void testkapp3()
{
    while (1)
        drawrectangle(40, bh, 20, 100, 0xFF00FF);
    return;
}

void testkapp4()
{
    while (1)
        drawrectangle(60, bh, 20, 100, 0x00FFFF);
    return;
}

void testkapp5()
{
    while (1)
        drawrectangle(80, bh, 20, 100, 0x00FF00);
    return;
}

void testkapp6()
{
    while (1)
        drawrectangle(100, bh, 20, 100, 0xFF0000);
    return;
}

void testkapp21()
{
    while (1)
        drawrectangle(0, bh + 100, 20, 100, 0xFF0000);
    return;
}

void testkapp22()
{
    while (1)
        drawrectangle(20, bh + 100, 20, 100, 0x00FF00);
    return;
}

void testkapp23()
{
    while (1)
        drawrectangle(40, bh + 100, 20, 100, 0x00FFFF);
    return;
}

void testkapp24()
{
    while (1)
        drawrectangle(60, bh + 100, 20, 100, 0xFF00FF);
    return;
}

void testkapp25()
{
    while (1)
        drawrectangle(80, bh + 100, 20, 100, 0xFFFF00);
    return;
}

void testkapp26()
{
    while (1)
        drawrectangle(100, bh + 100, 20, 100, 0xFFFFFF);
    return;
}

void testcolorchanger()
{
    int color = 0x000000;
    int rcolor = 0xFFFFFF;
    while (1)
    {
        if (color == 0xFFFFFF)
            color = 0x000000;
        else
            color++;

        if (rcolor == 0x000000)
            rcolor = 0xFFFFFF;
        else
            rcolor--;
        drawrectangle(120, bh, 20, 50, color);
        drawrectangle(120, bh + 50, 20, 50, rcolor);
    }
    return;
}

void testoverlapcolorchanger()
{
    int color = 0x000000;
    int rcolor = 0xFFFFFF;
    while (1)
    {
        if (color == 0xFFFFFF)
            color = 0x000000;
        else
            color++;

        if (rcolor == 0x000000)
            rcolor = 0xFFFFFF;
        else
            rcolor--;
        drawrectangle(140, bh, 20, 100, color);
        drawrectangle(140, bh, 20, 100, rcolor);
    }
    return;
}

void testmovetest()
{
    uint64_t wdth = CurrentDisplay->GetFramebuffer()->Width;
    uint64_t hght = CurrentDisplay->GetFramebuffer()->Height;
    int x = 0x0;
    while (1)
    {
        if (x > 5)
            drawrectangle(x - 5, hght - 20, 5, 20, 0x662222);
        drawrectangle(x, hght - 20, 5, 20, 0xFFFFFF);
        x++;
        if (x >= wdth - 5)
            x = 0x0;
    }
    return;
}

void KernelTask()
{
    printf("Kernel Compiled at: %s %s with C++ Standard: %d\n", __DATE__, __TIME__, CPP_LANGUAGE_STANDARD);
    printf("C++ Language Version (__cplusplus) :%ld\n", __cplusplus);

    bh = CurrentDisplay->GetFramebuffer()->Height / 2;

    address = CurrentDisplay->GetFramebuffer()->Address;
    width = CurrentDisplay->GetFramebuffer()->Width;
    height = CurrentDisplay->GetFramebuffer()->Height;
    ppsl = CurrentDisplay->GetFramebuffer()->PixelsPerScanLine;

    EnterCriticalSection;

    PCB *kmovetest = SysCreateProcess("MovingTest", Kernel);
    SysCreateThread(kmovetest, (uint64_t)testmovetest, 0, 0);

    PCB *kapp = SysCreateProcess("Test1", Kernel);
    SysCreateThread(kapp, (uint64_t)testkapp1, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp2, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp3, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp4, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp5, 0, 0);
    SysCreateThread(kapp, (uint64_t)testkapp6, 0, 0);

    PCB *kapp100 = SysCreateProcess("Test2", Kernel);
    SysCreateThread(kapp100, (uint64_t)testkapp21, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp22, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp23, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp24, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp25, 0, 0);
    SysCreateThread(kapp100, (uint64_t)testkapp26, 0, 0);

    PCB *kcolortest = SysCreateProcess("ColorTest", Kernel);
    SysCreateThread(kcolortest, (uint64_t)testcolorchanger, 0, 0);

    PCB *koverlaptest = SysCreateProcess("OverlapTest", Kernel);
    SysCreateThread(koverlaptest, (uint64_t)testoverlapcolorchanger, 0, 0);

    LeaveCriticalSection;

    // SysCreateProcessFromFile("/system/test1", 0, 0, User);
    // SysCreateProcessFromFile("/system/test2", 0, 0, User);
    // SysCreateProcessFromFile("/system/test3", 0, 0, User);
    // SysCreateProcessFromFile("/system/test4", 0, 0, User);
    trace("End Of Kernel Task");
    uint64_t rwdth = CurrentDisplay->GetFramebuffer()->Width;
    uint64_t rhght = CurrentDisplay->GetFramebuffer()->Height;
    CurrentDisplay->SetPrintColor(0x22AAFF);
    while (1)
    {
        drawrectangle(0, bh, 120, 200, 0x000000);
        drawrectangle(0, 0, 120, 15, 0x282828);
        drawrectangle(0, rhght - 20, rwdth, 20, 0x662222);
        CurrentDisplay->ResetPrintPosition();
        printf("CURRENT SPEED: %d", apic->APIC_ONESHOT_MULTIPLIER);
    }
    CPU_STOP;
}

void KernelInit()
{
    trace("Early initialization completed.");
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
    CurrentDisplay->KernelPrint('.');
    printf("\nKERNEL EARLY INITIALIZATION COMPLETE\nRUNNING KERNEL TASK...\n");
    StartTasking((uint64_t)KernelTask);
    CPU_STOP;
}
