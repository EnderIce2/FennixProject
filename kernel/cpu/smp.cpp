#include "smp.hpp"
#include "../kernel.h"

SymmetricMultiprocessing::SMP *smp = nullptr;

static void InitializeCPU(GBPSMPInfo *Data)
{
    fixme("Symmetric Multiprocessing for CPU %d not implemented.", Data->ID);
}

namespace SymmetricMultiprocessing
{
    SMP::SMP()
    {
        trace("Initializing symmetric multiprocessing (%d Cores)", bootparams->smp.CPUCount);
        for (size_t i = 0; i < bootparams->smp.CPUCount; i++)
            InitializeCPU(&bootparams->smp.smp[i]);
    }

    SMP::~SMP()
    {
    }
}
