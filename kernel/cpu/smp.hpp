#pragma once

namespace SymmetricMultiprocessing
{
    class SMP
    {
        public:
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
