#pragma once
#include <types.h>
#include <asm.h>
#include <critical.hpp>
#include "timer.h"

extern "C" unsigned long readTSC();

namespace CPUSpeed
{
    class cpuspeed
    {
    private:
        unsigned long CurrentTSC, FinalTSC = 0;

    public:
        uint64_t GetCPUSpeedHz() { return FinalTSC / 2000 / 10; }
        uint64_t GetCPUSpeedKHz() { return FinalTSC / 2000 / 100; }
        uint64_t GetCPUSpeedMHz() { return FinalTSC / 2000 / 1000; }

        cpuspeed()
        {
            EnterCriticalSection;
            for (int i = 0; i < 200; i++)
            {
                CurrentTSC = tsc();
                msleep(1); // This should be changed to a more accurate method.
                FinalTSC += tsc() - CurrentTSC;
            }
            LeaveCriticalSection;
        }
        ~cpuspeed() {}
    };
}