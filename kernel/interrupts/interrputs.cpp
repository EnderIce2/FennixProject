#include <int.h>

void EndOfInterrupt(int interrupt)
{
    // TODO: implement for APIC too
    PIC_sendEOI(interrupt);
}
