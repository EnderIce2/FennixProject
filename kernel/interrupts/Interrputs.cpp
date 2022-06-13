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

static bool RegisteredInterrupts[255];

InterruptVector RegisterInterrupt(INTERRUPT_HANDLER Handler)
{
    for (int i = IRQ18; i < IRQ223; i++)
        if (!RegisteredInterrupts[i])
        {
            RegisteredInterrupts[i] = true;
            register_interrupt_handler(i, Handler);
            apic->RedirectIRQ(CurrentCPU->ID, i - IRQ0, 1);
            trace("Registered interrupt handler for IRQ%d.", i - IRQ0);
            return i;
        }
    err("No available interrupt vector found.");
    return -1;
}

bool RegisterInterrupt(INTERRUPT_HANDLER Handler, InterruptVector Vector, bool Override, bool RedirectIRQ, InterruptVector RedirectVector)
{
    if (!Override)
        if (Vector < IRQ18 || Vector > IRQ223)
        {
            err("Invalid interrupt vector %#lx.", Vector);
            return false;
        }
    if (!RegisteredInterrupts[Vector])
    {
        RegisteredInterrupts[Vector] = true;
        register_interrupt_handler(Vector, Handler);
        goto Success;
    }
    else if (Override)
    {
        RegisteredInterrupts[Vector] = true;
        register_interrupt_handler(Vector, Handler);
        goto Success;
    }
    err("No available interrupt for vector IRQ%d found (override? %s).",
        Vector - IRQ0, Override ? "yes" : "no");
    return false;
Success:
    if (RedirectIRQ)
    {
        if (RedirectVector == 0xDEADBEEF)
            apic->RedirectIRQ(CurrentCPU->ID, Vector - IRQ0, 1);
        else
            apic->RedirectIRQ(CurrentCPU->ID, RedirectVector, 1);
    }
    trace("Registered interrupt handler for IRQ%d.", Vector - IRQ0);
    return true;
}

// C compatibility
extern "C" bool CRegisterInterrupt(INTERRUPT_HANDLER Handler, InterruptVector Vector, bool Override)
{
    return RegisterInterrupt(Handler, Vector, Override);
}

bool UnregisterInterrupt(InterruptVector Vector)
{
    if (Vector < IRQ18 || Vector > IRQ223)
        return false;
    RegisteredInterrupts[Vector] = false;
    unregister_interrupt_handler(Vector);
    trace("IRQ%d unregistered.", Vector - IRQ0);
    return true;
}
