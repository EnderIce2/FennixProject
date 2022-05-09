#pragma once
#include "idt.h"

#include "../kernel.h"

#define CurrentProcessor              \
    ({ &CPUs[({                       \
           uint64_t ret;              \
           asm volatile("movq %%gs:[" \
                        "0"           \
                        "], %0"       \
                        : "=r"(ret)   \
                        :             \
                        : "memory");  \
           ret;                       \
       })]; })

namespace SymmetricMultiprocessing
{
    class SMP
    {
    public:
        struct CPUData
        {
            uint64_t ID;
            uint64_t LAPICID;
            bool Ready;
            TaskStateSegment *TSS;

            /* TODO: fxsr implementation */
            /* TODO: process implementation */
        };
        /**
         * @brief Construct a new SMP object
         *
         */
        SMP();
        /**
         * @brief Destroy the SMP object
         *
         */
        ~SMP();
    };
}

extern SymmetricMultiprocessing::SMP *smp;
extern SymmetricMultiprocessing::SMP::CPUData *CPUs;