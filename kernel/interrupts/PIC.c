#include <interrupts.h>
#include "pic.h"
#include "../timer/pit.h"
#include <int.h>
#include <asm.h>
#include <io.h>

void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);

void PIC_disable()
{
    debug("Disabling PIC...");
    // set PIC to one-shot mode
    outb(PIT_CMD, 0x28);
    IOWait();
    outb(PIT_COUNTER0, CMD_MODE0);
    IOWait();
    // make sure all IRQs are masked
    IRQ_set_mask(0);
    IRQ_set_mask(1);
    IRQ_set_mask(2);
    IRQ_set_mask(3);
    IRQ_set_mask(4);
    IRQ_set_mask(5);
    IRQ_set_mask(6);
    IRQ_set_mask(7);
    IRQ_set_mask(8);
    IRQ_set_mask(9);
    IRQ_set_mask(10);
    IRQ_set_mask(11);
    IRQ_set_mask(12);
    IRQ_set_mask(13);
    IRQ_set_mask(14);
    IRQ_set_mask(15);
    outb(PIC1_DATA, 0xff);
    IOWait();
    outb(PIC2_DATA, 0xff);
    IOWait();
}

void PIC_sendEOI(unsigned char irq)
{
    if (irq >= 40)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_remap(int offset1, int offset2)
{
    debug("Remmaping PIC... %#llx %#llx", offset1, offset2);
    unsigned char a1, a2;
    a1 = inb(PIC1_DATA);
    IOWait();
    a2 = inb(PIC2_DATA);
    debug("%#llx %#llx", a1, a2);
    IOWait();
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();
    outb(PIC1_DATA, offset1);
    IOWait();
    outb(PIC2_DATA, offset2);
    IOWait();
    outb(PIC1_DATA, 4);
    IOWait();
    outb(PIC2_DATA, 2);
    IOWait();
    outb(PIC1_DATA, ICW4_8086);
    IOWait();
    outb(PIC2_DATA, ICW4_8086);
    IOWait();
    outb(PIC1_DATA, a1);
    IOWait();
    outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline)
{
    uint16_t port;
    uint8_t value;
    if (IRQline < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQline)
{
    uint16_t port;
    uint8_t value;
    if (IRQline < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}