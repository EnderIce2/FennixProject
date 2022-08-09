#include "tsc.h"
#include "../cpu/cpuid.h"
#include <asm.h>

void TSC_sleep(uint64_t Nanoseconds)
{
    if (cpu_feature(CPUID_FEAT_RDX_TSC))
    {
        // TODO: Calibrate the TSC timer
        static int once = 0;
        if (!once++)
            warn("Sleeping using TSC is not fully supported.");
        uint64_t start = tsc();
        while (tsc() - start < Nanoseconds)
            ;
    }
    else
    {
        warn("RDTSC is not supported.");
    }
}

void TSC_oneshot(uint32_t Vector, uint64_t Miliseconds)
{
    err("TSC One-Shot is not implemented!");
}
