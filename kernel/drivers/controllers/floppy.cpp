#include "floppy.h"
#include <interrupts.h>
#include <io.h>
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

    extern "C"
    {
        InterruptHandler(HandleFloppyDrive)
        {
            ReceivedIRQ = true;
            (void)(regs);
        }

        __attribute__((naked, used)) void floppy_driver_handler_helper()
        {
            asm("cld\n"
                "pushq %rax\n"
                "pushq %rbx\n"
                "pushq %rcx\n"
                "pushq %rdx\n"
                "pushq %rsi\n"
                "pushq %rdi\n"
                "pushq %rbp\n"
                "pushq %r8\n"
                "pushq %r9\n"
                "pushq %r10\n"
                "pushq %r11\n"
                "pushq %r12\n"
                "pushq %r13\n"
                "pushq %r14\n"
                "pushq %r15\n"
                "movq %ds, %rax\n"
                "pushq %rax\n"
                "movw $16, %ax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "movw %ax, %ss\n"
                "movq %rsp, %rdi\n"
                "call HandleFloppyDrive\n"
                "popq %rax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "popq %r15\n"
                "popq %r14\n"
                "popq %r13\n"
                "popq %r12\n"
                "popq %r11\n"
                "popq %r10\n"
                "popq %r9\n"
                "popq %r8\n"
                "popq %rbp\n"
                "popq %rdi\n"
                "popq %rsi\n"
                "popq %rdx\n"
                "popq %rcx\n"
                "popq %rbx\n"
                "popq %rax\n"
                "addq $16, %rsp\n"
                "iretq");
        }
    }

    FloppyDriver::FloppyDriver()
    {
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
