#include "pit.h"
#include <debug.h>
#include <io.h>
#include "../timer.h"

bool PIC_initialized = false;
uint64_t TimeSinceBoot = 0;
const uint32_t freq = 1193182;
uint16_t hertz = 1000;

uint32_t get_freq()
{
    return freq;
}

// InterruptHandler(pit_interrupt_handler)
// {
//     ticks++;
//     TimeSinceBoot += 1 / (freq / hertz); // not really sure if it works
// }

void init_pit()
{
    trace("Initializing PIT timer");
    uint32_t divisor = freq / hertz;
    outb(PIT_CMD, CMD_BINARY | CMD_MODE3 | CMD_RW_BOTH | CMD_COUNTER0);
    outb(PIT_COUNTER0, divisor);
    outb(PIT_COUNTER0, divisor >> 8);
    PIC_initialized = true;
}

void pit_wait(uint32_t miliseconds)
{
    uint32_t now = ticks;
    ++miliseconds;
    if (ticks <= 0)
    {
        err("PIT timer is not enabled!!");
    }
    else
        while (ticks - now < miliseconds)
            ;
}

uint64_t pit_get_uptime()
{
    return TimeSinceBoot;
}

// double TimeSinceBoot = 0;
// const uint64_t BaseFrequency = 1193182;
// uint16_t Divisor = 65535;

// void Sleepd(double seconds)
// {
//     double startTime = TimeSinceBoot;
//     while (TimeSinceBoot < startTime + seconds)
//     {
//         HLT;
//     }
// }

// void Sleep(uint64_t milliseconds)
// {
//     Sleepd((double)milliseconds / 1000);
// }

// void SetDivisor(uint16_t Divisor)
// {
//     if (Divisor < 100)
//         Divisor = 100;
//     Divisor = Divisor;
//     outb(0x40, (uint8_t)(Divisor & 0x00ff));
//     IOWait();
//     outb(0x40, (uint8_t)((Divisor & 0xff00) >> 8));
// }

// uint64_t GetFrequency()
// {
//     return BaseFrequency / Divisor;
// }

// void SetFrequency(uint64_t frequency)
// {
//     SetDivisor(BaseFrequency / frequency);
// }

// void Tick()
// {
//     TimeSinceBoot += 1 / (double)GetFrequency();
// }