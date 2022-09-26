#include <int.h>
#include "../cpu/apic.hpp"
#include "../cpu/idt.h"
#include "../cpu/smp.hpp"
#include "pic.h"

void EndOfInterrupt(int interrupt)
{
#if defined(__amd64__) || defined(__i386__)
    if (apic)
        if (apic->APICSupported())
        {
            apic->EOI();
            return;
        }
    PIC_sendEOI(interrupt);
#endif
}

/* -------------------------------------------------------------------------------------------------------------------------------- */

INTERRUPT_HANDLER MainRegisteredInterrupts[256];
DriverInterrupts::Register *MainDriverRegisteredInterrupts[256];

extern "C" void MainInterruptHandler(TrapFrame *regs)
{
#if defined(__amd64__) || defined(__i386__)
    if (((long)((int32_t)regs->int_num)) < ISR0 || regs->int_num > IRQ223)
    {
        err("Invalid interrupt received %#llx", regs->int_num);
        goto EndOfHandler;
    }

    if (MainDriverRegisteredInterrupts[regs->int_num] != nullptr)
    {
        MainDriverRegisteredInterrupts[regs->int_num]->HandleInterrupt(regs);
        goto EndOfHandler;
    }

    if (MainRegisteredInterrupts[regs->int_num] != nullptr)
    {
        MainRegisteredInterrupts[regs->int_num](regs);
        goto EndOfHandler;
    }

    err("IRQ%d is not registered!", regs->int_num - IRQ0);
#endif

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

/* -------------------------------------------------------------------------------------------------------------------------------- */

namespace DriverInterrupts
{
    Register::Register(InterruptVector Vector)
    {
        if (Vector < ISR0 || Vector > IRQ223)
        {
            err("Invalid interrupt vector %#llx", Vector);
            return;
        }
        IVector = Vector;
        debug("Registering driver interrupt handler for IRQ%d.", Vector - IRQ0);
        MainDriverRegisteredInterrupts[Vector] = this;
    }

    Register::~Register()
    {
        debug("Unregistering driver interrupt handler for IRQ%d.", IVector - IRQ0);
        MainDriverRegisteredInterrupts[IVector] = nullptr;
    }

    void Register::HandleInterrupt(TrapFrame *regs)
    {
        warn("Unhandled interrupt %#lx.", regs->int_num);
    }
}

/* -------------------------------------------------------------------------------------------------------------------------------- */

static bool RegisteredInterrupts[255];

InterruptVector RegisterInterrupt(INTERRUPT_HANDLER Handler)
{
    for (int i = IRQ18; i < IRQ223; i++)
        if (!RegisteredInterrupts[i])
        {
            RegisteredInterrupts[i] = true;
            MainRegisterInterrupt(i, Handler);
#if defined(__amd64__) || defined(__i386__)
            apic->RedirectIRQ(CurrentCPU->ID, i - IRQ0, 1);
#endif
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
        MainRegisterInterrupt(Vector, Handler);
        goto Success;
    }
    else if (Override)
    {
        RegisteredInterrupts[Vector] = true;
        MainRegisterInterrupt(Vector, Handler);
        goto Success;
    }
    err("No available interrupt for vector IRQ%d found (override? %s).",
        Vector - IRQ0, Override ? "yes" : "no");
    return false;
Success:
    if (RedirectIRQ)
    {
#if defined(__amd64__) || defined(__i386__)
        if (RedirectVector == 0xDEADBEEF)
            apic->RedirectIRQ(CurrentCPU->ID, Vector - IRQ0, 1);
        else
            apic->RedirectIRQ(CurrentCPU->ID, RedirectVector, 1);
#endif
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
    MainUnregisterInterrupt(Vector);
    trace("IRQ%d unregistered.", Vector - IRQ0);
    return true;
}
