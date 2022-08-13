#include <int.h>
#include "../cpu/apic.hpp"
#include "../cpu/idt.h"
#include "../cpu/smp.hpp"
#include "pic.h"

void EndOfInterrupt(int interrupt)
{
    if (apic)
        if (apic->APICSupported())
        {
            apic->EOI();
            return;
        }
    PIC_sendEOI(interrupt);
}

/* -------------------------------------------------------------------------------------------------------------------------------- */

INTERRUPT_HANDLER MainRegisteredInterrupts[256];

extern "C" void MainInterruptHandler(TrapFrame *regs)
{
    if (((long)((int32_t)regs->int_num)) < ISR0 || regs->int_num > IRQ223)
    {
        err("Invalid interrupt received %#llx", regs->int_num);
        goto EndOfHandler;
    }

    if (MainRegisteredInterrupts[regs->int_num] != nullptr)
    {
        MainRegisteredInterrupts[regs->int_num](regs);
        goto EndOfHandler;
    }

    err("IRQ%d is not registered!", regs->int_num - IRQ0);

EndOfHandler:
    EndOfInterrupt(regs->int_num);
}

void MainRegisterInterrupt(uint8_t vector, INTERRUPT_HANDLER handle)
{
    MainRegisteredInterrupts[vector] = handle;
    debug("Vector %#llx(IRQ%d) has been registered to handle %#llx", vector, vector - IRQ0, handle);
}

void MainUnregisterInterrupt(uint8_t vector)
{
    MainRegisteredInterrupts[vector] = nullptr;
    debug("Vector %#llx(IRQ%d) has been unregistered", vector, vector - IRQ0);
}
