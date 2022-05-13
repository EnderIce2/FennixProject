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
    // TODO: implement for APIC too
    PIC_sendEOI(interrupt);
}
