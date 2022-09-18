#pragma once
#include <io.h>

namespace PS2Mouse
{
    enum MouseButton
    {
        MouseNone,
        Right,
        Middle,
        Left
    };

    struct MouseInfo
    {
        MouseButton Buttons;
        long X;
        long Y;
    };

    class PS2MouseDriver
    {
    private:
        enum Config
        {
            READ_CONFIG = 0x20,
            WRITE_CONFIG = 0x60
        };

        enum Ports
        {
            DATA = 0x60,
            STATUS = 0x64,
            COMMAND = 0x64,
        };

        enum State
        {
            OUTPUT_FULL = (1 << 0),
            INPUT_FULL = (1 << 1),
            MOUSE_BYTE = (1 << 5)
        };

        void WaitRead()
        {
            uint64_t Timeout = 100000;
            while (Timeout--)
                if (inb(Ports::STATUS) & State::OUTPUT_FULL)
                    return;
        }

        void WaitWrite()
        {
            uint64_t Timeout = 100000;
            while (Timeout--)
                if ((inb(Ports::STATUS) & State::INPUT_FULL) == 0)
                    return;
        }

        void Write(uint16_t Port, uint8_t Value)
        {
            WaitWrite();
            outb(Port, Value);
        }

        uint8_t Read()
        {
            WaitRead();
            return inb(Ports::DATA);
        }

    public:
        MouseInfo GetMouseInfo();
        PS2MouseDriver();
        ~PS2MouseDriver();
    };
}

extern PS2Mouse::PS2MouseDriver *ps2mouse;
