#include <interrupts.h>
#include <int.h>
#include <asm.h>
#include <io.h>

void PIC_disable()
{
    debug("Disabling PIC...");
    // /* Set ICW1 */
    // outb(PIC1_COMMAND, 0x11);
    // IOWait();
    // outb(PIC2_COMMAND, 0x11);
    // IOWait();
    // /* Set ICW2 (IRQ base offsets) */
    // outb(PIC1_DATA, 0xe0);
    // IOWait();
    // outb(PIC2_DATA, 0xe8);
    // IOWait();
    // /* Set ICW3 */
    // outb(PIC1_DATA, 4);
    // IOWait();
    // outb(PIC2_DATA, 2);
    // IOWait();
    // /* Set ICW4 */
    // outb(PIC1_DATA, 1);
    // IOWait();
    // outb(PIC2_DATA, 1);
    // IOWait();
    // /* Set OCW1 (interrupt masks) */
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