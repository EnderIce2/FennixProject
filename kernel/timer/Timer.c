#include "../timer.h"
#include <interrupts.h>
#include <int.h>
#include <asm.h>
#include "apic_timer.h"
#include "hpet.h"
#include "pit.h"
#include "tsc.h"
#include "../cpu/idt.h"

struct ActiveTimers active_timers;

void nsleep(uint64_t Nanoseconds)
{
    if (APICTimer_initialized)
        apictimer_nwait(Nanoseconds);
    else if (HPET_initialized)
    {
        static int once = 0;
        if (!once++)
            warn("HPET is not supposed to work with nanoseconds!");
        hpet_uwait(Nanoseconds);
    }
    else if (PIC_initialized)
    {
        static int once = 0;
        if (!once++)
            warn("PIT is not supposed to work with nanoseconds!");
        pit_wait(Nanoseconds / 1000);
    }
    else
    {
        TSC_sleep(Nanoseconds);
    }
}

void usleep(uint64_t Microseconds)
{
    if (APICTimer_initialized)
        apictimer_uwait(Microseconds);
    else if (HPET_initialized)
        hpet_uwait(Microseconds);
    else if (PIC_initialized)
    {
        static int once = 0;
        if (!once++)
            warn("PIT is not supposed to work with microseconds!");
        pit_wait(Microseconds / 1000);
    }
    else
        nsleep(Microseconds * 1000);
}

void msleep(uint64_t Miliseconds)
{
    if (APICTimer_initialized)
        apictimer_mwait(Miliseconds);
    else if (HPET_initialized)
        hpet_mwait(Miliseconds);
    else if (PIC_initialized)
        pit_wait(Miliseconds);
    else
        usleep(Miliseconds * 1000);
}

void sleep(uint64_t Seconds)
{
    if (APICTimer_initialized)
        apictimer_wait(Seconds);
    else if (HPET_initialized)
        hpet_wait(Seconds);
    else if (PIC_initialized)
        pit_wait(Seconds * 1000);
    else
        msleep(Seconds * 1000000);
}

volatile uint64_t ticks = 0;

uint64_t counter()
{
    if (APICTimer_initialized)
        return apictimer_read_counter();
    else if (HPET_initialized)
        return hpet_read_counter();
    else
        return ticks;
}

uint32_t get_timer_clock()
{
    if (APICTimer_initialized)
        return apictimer_read_clock();
    else if (HPET_initialized)
        return get_clk();
    else
        return get_freq();
}

void TimerOneShot(uint32_t Vector, uint64_t Miliseconds)
{
    if (APICTimer_initialized)
        APIC_oneshot(Vector, Miliseconds);
    else if (HPET_initialized)
        HPET_oneshot(Vector, Miliseconds);
    else if (PIC_initialized)
        pit_oneshot(Vector, Miliseconds);
    else
        TSC_oneshot(Vector, Miliseconds);
}

InterruptHandler(timer_interrupt_handler) { ticks++; }

void init_timer()
{
    // TODO: apic timer interrupt handler is called only once
    CRegisterInterrupt(timer_interrupt_handler, IRQ0, true);
    init_APICTimer();
    if (!APICTimer_initialized)
    {
        init_HPET();
        if (!HPET_initialized)
            init_pit();
    }

    if (APICTimer_initialized)
    {
        active_timers.APIC = true;
        debug("Timer APIC has been enabled.");
    }
    else if (HPET_initialized)
    {
        active_timers.HPET = true;
        debug("Timer HPET has been enabled.");
    }
    else if (PIC_initialized)
    {
        active_timers.PIT = true;
        debug("Timer PIT has been enabled.");
    }
    else
    {
        active_timers.TSC = true;
        debug("Timer TSC has been enabled.");
    }
}
