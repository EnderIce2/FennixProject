#include "mouse.hpp"

#include "../cpu/idt.h"
#include "../kernel.h"

#include <display.h>
#include <filesystem.h>
#include <interrupts.h>
#include <critical.hpp>
#include <int.h>
#include <io.h>

PS2Mouse::PS2MouseDriver *ps2mouse = nullptr;

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

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton 0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

    uint8_t Packet[4];
    bool PacketReady = false;
    MouseInfo minfo = {MouseNone, 0, 0};

    void ProcessMousePacket()
    {
        if (!PacketReady)
            return;

        bool XNegative, YNegative, XOverflow, YOverflow;

        if (Packet[0] & PS2XSign)
            XNegative = true;
        else
            XNegative = false;

        if (Packet[0] & PS2YSign)
            YNegative = true;
        else
            YNegative = false;

        if (Packet[0] & PS2XOverflow)
            XOverflow = true;
        else
            XOverflow = false;

        if (Packet[0] & PS2YOverflow)
            YOverflow = true;
        else
            YOverflow = false;

        if (!XNegative)
        {
            minfo.X += Packet[1];
            if (XOverflow)
                minfo.X += 255;
        }
        else
        {
            Packet[1] = 256 - Packet[1];
            minfo.X -= Packet[1];
            if (XOverflow)
                minfo.X -= 255;
        }

        if (!YNegative)
        {
            minfo.Y -= Packet[2];
            if (YOverflow)
                minfo.Y -= 255;
        }
        else
        {
            Packet[2] = 256 - Packet[2];
            minfo.Y += Packet[2];
            if (YOverflow)
                minfo.Y += 255;
        }

        if (minfo.X < 0)
            minfo.X = 0;
        if (minfo.X > CurrentDisplay->GetFramebuffer()->Width - 1)
            minfo.X = CurrentDisplay->GetFramebuffer()->Width - 1;

        if (minfo.Y < 0)
            minfo.Y = 0;
        if (minfo.Y > CurrentDisplay->GetFramebuffer()->Height - 1)
            minfo.Y = CurrentDisplay->GetFramebuffer()->Height - 1;

        if (Packet[0] & PS2Leftbutton)
            minfo.Buttons = MouseButton::Left;
        if (Packet[0] & PS2Middlebutton)
            minfo.Buttons = MouseButton::Middle;
        if (Packet[0] & PS2Rightbutton)
            minfo.Buttons = MouseButton::Right;
        PacketReady = false;
    }

    extern "C"
    {
        uint8_t Cycle = 0;
        InterruptHandler(PS2MouseInterruptHandler)
        {
            EnterCriticalSection;
            uint8_t Data = inb(0x60);
            ProcessMousePacket();

            switch (Cycle)
            {
            case 0:
            {
                if ((Data & 0b00001000) == 0)
                    break;
                Packet[0] = Data;
                Cycle++;
                break;
            }
            case 1:
            {
                Packet[1] = Data;
                Cycle++;
                break;
            }
            case 2:
            {
                Packet[2] = Data;
                PacketReady = true;
                Cycle = 0;
                break;
            }
            }
            LeaveCriticalSection;
            EndOfInterrupt(INT_NUM);
        }
    }

    ReadFSFunction(Mouse_Read)
    {
        memcpy(Buffer, &minfo, Size);
        return Size;
    }

    FileSystem::FileSystemOpeations mouse = {
        .Name = "PS/2 Mouse",
        .Read = Mouse_Read};

    PS2MouseDriver::PS2MouseDriver()
    {
        outb(COMMAND, 0xA8);
        Write(COMMAND, READ_CONFIG);
        uint8_t Status = Read();
        Status |= 0b10;
        Write(COMMAND, WRITE_CONFIG);
        Write(DATA, Status);
        Write(COMMAND, 0xD4);
        Write(DATA, 0xF6);
        Read();
        Write(COMMAND, 0xD4);
        Write(DATA, 0xF4);
        Read();
        register_interrupt_handler(IRQ12, PS2MouseInterruptHandler);
        devfs->AddFileSystem(&mouse, 0666, "mouse", FileSystem::NodeFlags::FS_PIPE);
    }

    PS2MouseDriver::~PS2MouseDriver()
    {
        warn("Unloading default mouse driver is not supported.");
    }
}