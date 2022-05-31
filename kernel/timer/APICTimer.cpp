#include "apic_timer.h"

#include <int.h>

#include "../cpu/apic.hpp"
#include "../cpu/idt.h"
#include "../timer.h"
#include "hpet.h"

bool APICTimer_initialized = false;
uint64_t apic_timer_ticks = 0;

uint64_t apictimer_read_counter()
{
    return hpet_read_counter();
    // return ((0xFFFFFFFF - apic->Read(APIC::APIC::TCCR)) / 100);
}

uint64_t apictimer_read_clock()
{
    return get_clk();
    // return apic->Read(APIC::APIC::TDCR);
}

void apictimer_nwait(uint64_t Nanoseconds)
{
    uint64_t target = apictimer_read_counter() + (Nanoseconds / apictimer_read_clock());
    while (apictimer_read_counter() < target)
        ;
}

void apictimer_uwait(uint64_t Microseconds)
{
    hpet_uwait(Microseconds);
}

void apictimer_mwait(uint64_t Miliseconds)
{
    hpet_mwait(Miliseconds);
}

void apictimer_wait(uint64_t Seconds)
{
    hpet_wait(Seconds);
}

InterruptHandler(hpet_stub_interrupt)
{
    EndOfInterrupt(INT_NUM);
}

void init_APICTimer()
{
    trace("Initializing APIC Timer...");
    register_interrupt_handler(IRQ2, hpet_stub_interrupt);
    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    init_HPET();

    apic->Write(APIC::APIC::APIC_TICR, 0xFFFFFFFF);

    usleep(100);

    apic->Write(APIC::APIC::APIC_TIMER, 0x10000);
    apic_timer_ticks = 0xFFFFFFFF - apic->Read(APIC::APIC::APIC_TCCR);
    apic_timer_ticks /= 100;

    disable_HPET();
    apic->Write(APIC::APIC::APIC_TIMER, 32 | APIC::APIC::APICRegisters::APIC_PERIODIC);
    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    apic->Write(APIC::APIC::APIC_TICR, apic_timer_ticks);

    APICTimer_initialized = true;
}
