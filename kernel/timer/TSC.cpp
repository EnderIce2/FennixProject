#include "tsc.h"
#include "../cpu/cpuid.h"
#include <asm.h>

void TSC_sleep(uint64_t Nanoseconds)
{
    if (cpu_feature(CPUID_FEAT_RDX_TSC))
    {
        // TODO: Calibrate the TSC timer
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
