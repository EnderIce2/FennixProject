#include "serial.h"
#include <lock.h>
#include <io.h>

NEWLOCK(serial_lock);

// Source from: https://wiki.osdev.org/Serial_Ports

int init_serial(int serial_port)
{
    SILENT_LOCK(serial_lock);
    outb(serial_port + 1, 0x00);                 // Disable all interrupts
    outb(serial_port + 3, SERIAL_ENABLE_DLAB);   // Enable DLAB (set baud rate divisor)
    outb(serial_port + 0, SERIAL_RATE_38400_LO); // Set divisor to 3 (lo byte) 38400 baud
    outb(serial_port + 1, SERIAL_RATE_38400_HI); //                  (hi byte)
    outb(serial_port + 3, 0x03);                 // 8 bits, no parity, one stop bit
    outb(serial_port + 2, 0xC7);                 // Enable FIFO, clear them, with 14-byte threshold
    outb(serial_port + 4, 0x0B);                 // IRQs enabled, RTS/DSR set
    outb(serial_port + 4, 0x1E);                 // Set in loopback mode, test the serial chip
    outb(serial_port + 0, 0xAE);                 // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(serial_port + 0) != 0xAE)
    {
        SILENT_UNLOCK(serial_lock);
        return -1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(serial_port + 4, 0x0F);
    SILENT_UNLOCK(serial_lock);
    return 0;
}

int is_transmit_empty(int serial_port)
{
    return inb(serial_port + 5) & SERIAL_BUFFER_EMPTY;
}

int serial_received(int serial_port)
{
    return inb(serial_port + 5) & 1;
}

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
    // SILENT_LOCK(serial_lock);
    for (long unsigned i = 0; i < strlen(text); i++)
    {
        write_serial(serial_port, text[i]);
    }
    // SILENT_UNLOCK(serial_lock);
}