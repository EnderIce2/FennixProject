#pragma once

namespace PS2Keyboard
{
    class PS2KeyboardDriver
    {
    public:
        char GetLastScanCode();
        PS2KeyboardDriver();
        ~PS2KeyboardDriver();
    };
}

extern PS2Keyboard::PS2KeyboardDriver *ps2keyboard;