#include "apic_timer.h"

#include <int.h>
#include <asm.h>
#include <io.h>

#include "../cpu/apic.hpp"
#include "../cpu/idt.h"
#include "../timer.h"
#include "hpet.h"
#include "pit.h"

bool APICTimer_initialized = false;
uint64_t apic_timer_ticks = 0;

uint64_t apictimer_read_counter() { return 0xFFFFFFFF - apic->Read(APIC::APIC::APIC_TCCR); }

uint64_t apictimer_read_clock() { return apic_timer_ticks; }

void apictimer_nwait(uint64_t Nanoseconds)
{
    outb(PIT_CMD, 0x30);
    uint16_t Wait100ms = 1193182 / Nanoseconds;
    outb(PIT_COUNTER0, (uint8_t)(Wait100ms & 0xFF));
    outb(PIT_COUNTER0, (uint8_t)((Wait100ms >> 8) & 0xFF));
}

void apictimer_uwait(uint64_t Microseconds)
{
    apictimer_nwait(Microseconds * 1000);
}

void apictimer_mwait(uint64_t Miliseconds)
{
    apictimer_uwait(Miliseconds * 1000);
}

void apictimer_wait(uint64_t Seconds)
{
    apictimer_mwait(Seconds * 1000);
}

extern "C" uint32_t ReadPITCounter();

void init_APICTimer()
{
    trace("Initializing APIC Timer...");

    uint16_t Divisor = 1193182 / 1000;
    outb(PIT_CMD, 0x36);
    IOWait();
    outb(PIT_COUNTER0, (uint8_t)(Divisor & 0xFF)); // low
    IOWait();
    outb(PIT_COUNTER0, (uint8_t)((Divisor >> 8) & 0xFF)); // high
    IOWait();

    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    apic->Write(APIC::APIC::APIC_TICR, 0xFFFFFFFF);

    outb(PIT_CMD, 0x30);
    uint16_t Wait100ms = 1193182 / (10 * 1000);
    outb(PIT_COUNTER0, (uint8_t)(Wait100ms & 0xFF));
    outb(PIT_COUNTER0, (uint8_t)((Wait100ms >> 8) & 0xFF));

    while (1)
        if (ReadPITCounter() == 0)
            break;

    apic->Write(APIC::APIC::APIC_TIMER, 0x10000);
    apic_timer_ticks = 0xFFFFFFFF - apic->Read(APIC::APIC::APIC_TCCR);
    apic_timer_ticks /= 100;

    apic->Write(APIC::APIC::APIC_TIMER, 32 | APIC::APIC::APICRegisters::APIC_PERIODIC);
    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    apic->Write(APIC::APIC::APIC_TICR, apic_timer_ticks);

    APICTimer_initialized = true;
}
