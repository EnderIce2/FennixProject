#include "keyboard.hpp"

#include "../kernel.h"

#include <interrupts.h>
#include <int.h>
#include <asm.h>
#include <io.h>

PS2Keyboard::PS2KeyboardDriver *ps2keyboard = nullptr;

namespace PS2Keyboard
{
    static uint8_t LastSC;

    extern "C"
    {
        InterruptHandler(PS2KeyboardInterruptHandler)
        {
            uint8_t scanCode = inb(0x60);
            LastSC = scanCode;
            if (scanCode == 0x3B)
                ShowRecoveryScreen = true;
        }
    }

    uint8_t PS2KeyboardDriver::GetLastScanCode() { return LastSC; }

    uint8_t PS2KeyboardDriver::WaitScanCode()
    {
        LastSC = 0x0;
        while (!LastSC)
            HLT;
        return LastSC;
    }

    PS2KeyboardDriver::PS2KeyboardDriver()
    {
        RegisterInterrupt(PS2KeyboardInterruptHandler, IRQ1, true);

        while (inb(0x64) & 0x1)
            inb(0x60);

        outb(0x64, 0xAE);
        outb(0x64, 0x20);
        uint8_t ret = (inb(0x60) | 1) & ~0x10;
        outb(0x64, 0x60);
        outb(0x60, ret);
        outb(0x60, 0xF4);
    }

    PS2KeyboardDriver::~PS2KeyboardDriver()
    {
        warn("Unloading default keyboard driver is not supported.");
    }
}
