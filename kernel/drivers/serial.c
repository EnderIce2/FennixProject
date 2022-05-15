#include "serial.h"
#include <lock.h>
#include <io.h>

NEWLOCK(serial_lock);

// Source from: https://wiki.osdev.org/Serial_Ports

int init_serial(int serial_port)
{
    LOCK(serial_lock);
    outb(serial_port + 7, 0x55);
    if (inb(serial_port + 7) != 0x55)
    {
        UNLOCK(serial_lock);
        return -1;
    }

    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, SERIAL_ENABLE_DLAB);
    outb(serial_port + 0, 0x01);
    outb(serial_port + 1, SERIAL_RATE_38400_HI);
    outb(serial_port + 3, 0x03);
    outb(serial_port + 2, 0xC7);
    outb(serial_port + 4, 0x0B);

    if (inb(serial_port + 0) != 0xAE)
    {
        UNLOCK(serial_lock);
        return -1;
    }

    outb(serial_port + 4, 0x0F);
    UNLOCK(serial_lock);
    return 0;
}

int is_transmit_empty(int serial_port) { return inb(serial_port + 5) & SERIAL_BUFFER_EMPTY; }

int serial_received(int serial_port) { return inb(serial_port + 5) & 1; }

char read_serial(int serial_port)
{
    while (serial_received(serial_port) == 0)
        ;
    return inb(serial_port);
}

void write_serial(int serial_port, char a)
{
    while (is_transmit_empty(serial_port) == 0)
        ;
    outb(serial_port, a);
}

void serial_write_text(int serial_port, char *text)
{
    // LOCK(serial_lock);
    for (long unsigned i = 0; i < (strlen(text)); i++)
        write_serial(serial_port, text[i]);
    // UNLOCK(serial_lock);
}
