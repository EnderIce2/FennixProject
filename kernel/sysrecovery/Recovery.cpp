#include "recovery.hpp"

#include <internal_task.h>
#include <display.h>
#include <asm.h>
#include <io.h>

#include "../drivers/keyboard.hpp"
#include "../cpu/acpi.hpp"

namespace SystemRecovery
{
    Recovery::Recovery()
    {
        trace("Recovery mode triggered.");
        CLI;
        CurrentDisplay->Clear(0x101010);
        printf("%s - %s | Recovery Mode", KERNEL_NAME, KERNEL_VERSION);
        /* ... do stuff ... */
        uint8_t sc = 0;
        while (sc != 1) // esc
        {
            CLI;
            sc = ps2keyboard->GetLastScanCode();
            STI;
        }
        printf("\nRebooting...");
#if defined(__amd64__) || defined(__i386__)
        dsdt->reboot();
#endif
    }

    Recovery::~Recovery()
    {
    }
}
