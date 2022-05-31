#include <int.h>
#include "../cpu/apic.hpp"

void EndOfInterrupt(int interrupt)
{
    if (apic)
        if (apic->APICSupported())
        {
            apic->EOI();
            return;
        }
    PIC_sendEOI(interrupt);
}
