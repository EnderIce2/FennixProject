#include <types.h>

extern bool APICTimer_initialized;
extern uint64_t apic_timer_ticks;

EXTERNC uint64_t apictimer_read_counter();
EXTERNC uint64_t apictimer_read_clock();
EXTERNC void apictimer_nwait(uint64_t Nanoseconds);
EXTERNC void apictimer_uwait(uint64_t Microseconds);
EXTERNC void apictimer_mwait(uint64_t Miliseconds);
EXTERNC void apictimer_wait(uint64_t Seconds);
EXTERNC void init_APICTimer();
EXTERNC void APIC_oneshot(uint32_t Vector, uint64_t Miliseconds);
