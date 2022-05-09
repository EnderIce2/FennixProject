#include "recovery.hpp"
#include <asm.h>
#include <display.h>

namespace SystemRecovery
{
    Recovery::Recovery()
    {
        CurrentDisplay->Clear(0x282828);
        printf("TODO");
        CPU_STOP;
    }

    Recovery::~Recovery()
    {
    }
}
