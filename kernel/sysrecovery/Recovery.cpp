#include "recovery.hpp"

#include <display.h>
#include <asm.h>
#include <io.h>

#include "../drivers/keyboard.hpp"
#include "../cpu/acpi.hpp"

namespace SystemRecovery
{
    Recovery::Recovery()
    {
        CurrentDisplay->Clear(0x282828);
        printf("TODO");
        while (1)
        {
            asm volatile("sti");
            uint8_t sc = ps2keyboard->GetLastScanCode();
            if (sc == 0x1) // esc
                dsdt->reboot();
        }
        dsdt->reboot();
    }

    Recovery::~Recovery()
    {
    }
}
