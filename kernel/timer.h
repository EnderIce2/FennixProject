#ifndef __FENNIX_KERNEL_TIMER_H__
#define __FENNIX_KERNEL_TIMER_H__

#include <types.h>

struct ActiveTimers
{
    bool APIC;
    bool HPET;
    bool PIT;
    bool TSC;
};

extern struct ActiveTimers active_timers;

extern volatile uint64_t ticks;
EXTERNC void sleep(uint64_t Seconds);
EXTERNC void msleep(uint64_t Miliseconds);
EXTERNC void usleep(uint64_t Microseconds);
EXTERNC void nsleep(uint64_t Nanoseconds);
EXTERNC uint32_t get_timer_clock();
EXTERNC uint64_t counter();
EXTERNC void TimerOneShot(uint32_t Vector, uint64_t Miliseconds);
EXTERNC void init_timer();

#endif // !__FENNIX_KERNEL_TIMER_H__
