#include "floppy.h"
#include "../../cpu/idt.h"
#include <interrupts.h>
#include <io.h>
#include <int.h>
#include <asm.h>

int ReceivedIRQ = false;

namespace Floppy
{
#define DOR 0x3F2
#define CCR 0x3F7

    string FloppyDriver::ReturnFloppyType(int i)
    {
        switch (i)
        {
        case 0:
            return "No floppy drive.";
            break;
        case 1:
            return "360KB 5.25in floppy";
            break;
        case 2:
            return "1.2MB 5.25in floppy";
            break;
        case 3:
            return "720KB 3.5in floppy";
            break;
        case 4:
            return "1.44MB 3.5in floppy";
            break;
        case 5:
            return "2.88MB 3.5in floppy";
            break;
        default:
            return "error";
            break;
        }
    }

    void FloppyDriver::SendByte_FDC(char b)
    {
        unsigned char a = 0;
        while (true)
        {
            // usleep(100);
            a = inb(0x3F4);
            a &= 0x80;
            if (a == 0x80)
                break;
        }
        // usleep(100);
        outb(0x3F5, b);
    }

    unsigned char FloppyDriver::RecByte_FDC()
    {
        unsigned char a;
        unsigned char b;
        while (true)
        {
            // usleep(100);
            a = inb(0x3F4);
            a &= 0x80;
            if (a == 0x80)
                break;
        }
        b = inb(0x3F5);
        return b;
    }

    void FloppyDriver::Recal()
    {
        unsigned char res = 0;
        unsigned char cres = 0;
        while (true)
        {
            SendByte_FDC(7);
            SendByte_FDC(0);
            SendByte_FDC(0x08);
            res = RecByte_FDC();
            cres = res & 0x80;
            if (cres == 0x80)
                continue;
            res = RecByte_FDC();
            if (res == 0)
                break;
        }
    }

    void FloppyDriver::SeekTrackSide(unsigned char track, unsigned char side)
    {
        unsigned char a, b, c;
        while (true)
        {
            SendByte_FDC(0x0F);
            SendByte_FDC(side * 4);
            SendByte_FDC(track);
            SendByte_FDC(0x08);
            a = RecByte_FDC();
            a &= 0x80;
            if (a != 0x80)
                break;
            a = RecByte_FDC();
            if (a == track)
                break;
        }
    }

    void FloppyDriver::MotorOn() { outb(DOR, 0x1C); }

    void FloppyDriver::MotorOff() { outb(DOR, 0x0); }

    void FloppyDriver::WaitForIRQ()
    {
        ReceivedIRQ = false;
        while (ReceivedIRQ == false)
            HLT;
    }

    void FloppyDriver::ResetFloppy()
    {
        // TODO: as the osdev wiki said "Sure this code looks OK, but some emulators or floppy drives might manage to be faster than your code. What if you've just returned from EnableController() and the floppy already issued the IRQ6? Then ReceivedIRQ will be set to true, your driver will enter WaitForIRQ(), set it to false again and then infinitely loop, waiting for an IRQ that has already been received."
        outb(DOR, 0x00);
        outb(DOR, 0x0C);
        outb(CCR, 0x00);
        WaitForIRQ();
    }

    InterruptHandler(FloppyDriveHandler)
    {
        ReceivedIRQ = true;
        (void)(regs);
    }

    FloppyDriver::FloppyDriver()
    {
        RegisterInterrupt(FloppyDriveHandler, IRQ6, true);
        unsigned int c;
        outb(0x70, 0x10);
        c = inb(0x71);
        trace("1: %s", ReturnFloppyType(c >> 4));
        trace("2: %s", ReturnFloppyType(c & 0xF));
        if ((c >> 4) != 0)
            this->Drives[0].Name = "fd0";

        if ((c & 0xF) != 0)
            this->Drives[1].Name = "fd1";

        // TODO: implement read/writing to floppy
    }

    FloppyDriver::~FloppyDriver()
    {
        warn("Tried to uninitialize Floppy driver!");
    }

}
