#include "../timer.h"
#include <internal_task.h>
#include <interrupts.h>
#include <int.h>
#include "hpet.h"
#include "pit.h"
#include "../cpu/idt.h"

void sleep(uint64_t Seconds)
{
    if (HPET_initialized)
        hpet_wait(Seconds);
    else
        pit_wait(Seconds * 1000);
}

void msleep(uint64_t Miliseconds)
{
    if (HPET_initialized)
        hpet_mwait(Miliseconds);
    else
        pit_wait(Miliseconds);
}

void usleep(uint64_t Microseconds)
{
    if (HPET_initialized)
        hpet_uwait(Microseconds);
    else
    {
        static int once = 0;
        if (!once++)
            warn("PIT is not supposed to work with microseconds!");
        pit_wait(Microseconds / 1000);
    }
}

volatile uint64_t ticks = 0;

uint64_t counter()
{
    if (!HPET_initialized)
        return ticks;
    else
        return hpet_read_counter();
}

uint32_t get_timer_clock()
{
    if (HPET_initialized)
        return get_clk();
    else
        return get_freq();
}

uint64_t systemuptimeseconds = 0;
bool uptimesettarget = false;
uint64_t uptimecurrenttarget = 0;

uint64_t get_system_uptime()
{
    return systemuptimeseconds;
}

static uint64_t timeslice_target = 0;

void set_yield_schedule(uint64_t timeslice)
{
    timeslice_target = timeslice;
}

InterruptHandler(timer_interrupt_handler)
{
    ticks++;
    if (hpet_read_counter() > timeslice_target)
        schedule();
    if (HPET_initialized)
    {
        if (!uptimesettarget)
        {
            uptimecurrenttarget = hpet_read_counter() + ((uint64_t)1000000 * (uint64_t)1000000000) / get_clk();
            uptimesettarget = true;
        }
        if (hpet_read_counter() > uptimecurrenttarget)
        {
            systemuptimeseconds++;
            uptimesettarget = false;
        }
    }
    else
    {
        systemuptimeseconds += get_freq() / 1000; // TODO: check if this is correct
    }
    EndOfInterrupt(INT_NUM);
}

void init_timer()
{
    register_interrupt_handler(IRQ0, timer_interrupt_handler);
    init_HPET();
    if (!HPET_initialized)
        init_pit();
}