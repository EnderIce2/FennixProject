#include "apic_timer.h"

#include <int.h>
#include <asm.h>
#include <io.h>

#include "../cpu/apic.hpp"
#include "../cpu/idt.h"
#include "../timer.h"
#include "tsc.h"
#include "hpet.h"
#include "pit.h"

bool APICTimer_initialized = false;
uint64_t apic_timer_ticks = 32000;

uint64_t apictimer_read_counter() { return apic->Read(APIC::APIC::APIC_TCCR); }
uint64_t apictimer_read_clock() { return apic_timer_ticks; }
void apictimer_nwait(uint64_t Nanoseconds) { TSC_sleep(Nanoseconds); }
void apictimer_uwait(uint64_t Microseconds) { apictimer_nwait(Microseconds * 1000); }
void apictimer_mwait(uint64_t Miliseconds) { apictimer_uwait(Miliseconds * 1000); }
void apictimer_wait(uint64_t Seconds) { apictimer_mwait(Seconds * 1000); }

void init_APICTimer()
{
    trace("Initializing APIC Timer...");

    // Initializing the APIC timer corrups the memory? Or something else?

    apic->Write(APIC::APIC::APIC_TIMER, apic->Read(APIC::APIC::APIC_TIMER) & ~(1 << 0x10));

    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    apic->Write(APIC::APIC::APIC_TICR, 0xFFFFFFFF);

    apictimer_mwait(100);

    apic->Write(APIC::APIC::APIC_TIMER, 0x10000);
    apic_timer_ticks = 0xFFFFFFFF - apic->Read(APIC::APIC::APIC_TCCR);
    apic_timer_ticks /= 100;

    apic->Write(APIC::APIC::APIC_TIMER, (long)IRQ0 | (long)APIC::APIC::APICRegisters::APIC_PERIODIC);
    apic->Write(APIC::APIC::APIC_TDCR, 0x3);
    apic->Write(APIC::APIC::APIC_TICR, apic_timer_ticks);

    APICTimer_initialized = true;
    trace("APIC timer ticks %lld", apic_timer_ticks);
    apic->Write(APIC::APIC::APIC_TIMER, apic->Read(APIC::APIC::APIC_TIMER) | (1 << 0x10));
}

void APIC_oneshot(uint32_t Vector, uint64_t Miliseconds)
{
    apic->OneShot(Vector, Miliseconds);
}
