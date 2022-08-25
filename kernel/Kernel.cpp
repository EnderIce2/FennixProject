#include "kernel.h"

void port_byte_out(short unsigned int port, unsigned char value)
{
    __asm__ volatile("outb %0, %1"
                     :
                     : "a"(value), "Nd"(port));
}

unsigned char port_byte_in(short unsigned int port)
{
    unsigned char ReturnValue;
    __asm__ volatile("inb %1, %0"
                     : "=a"(ReturnValue)
                     : "Nd"(port));
    return ReturnValue;
}

int strlen(const char s[])
{
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

#define COM1 0x3F8

int init_serial()
{
    port_byte_out(COM1 + 1, 0x00);
    port_byte_out(COM1 + 3, 0x80);
    port_byte_out(COM1 + 0, 0x03);
    port_byte_out(COM1 + 1, 0x00);
    port_byte_out(COM1 + 3, 0x03);
    port_byte_out(COM1 + 2, 0xC7);
    port_byte_out(COM1 + 4, 0x0B);
    port_byte_out(COM1 + 4, 0x1E);
    port_byte_out(COM1 + 0, 0xAE);
    if (port_byte_in(COM1 + 0) != 0xAE)
    {
        return -1; // serial port is faulty
    }
    port_byte_out(COM1 + 4, 0x0F);
    return 0;
}

void SerialWrite(const char *text)
{
    for (int i = 0; i < strlen(text); i++)
    {
        while ((port_byte_in(COM1 + 5) & 0x20) == 0)
            ;
        port_byte_out(COM1, text[i]);
    }
}

extern "C" void kernel_entry(void *data)
{
    SerialWrite("Hello, World! Kernel started successfully.\n");
    while (1)
        __asm__ volatile("hlt");
}
