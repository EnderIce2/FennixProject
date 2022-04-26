#include <io.h>
#include <syscalls.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define SERIAL_ENABLE_DLAB 0x80
#define SERIAL_RATE_38400_LO 0x03
#define SERIAL_RATE_38400_HI 0x00
#define SERIAL_BUFFER_EMPTY 0x20

long unsigned strlen(char s[])
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void serial_write_text(char *text)
{
    for (int i = 0; i < (strlen(text)); i++)
    {
        while ((inb(COM1 + 5) & SERIAL_BUFFER_EMPTY) == 0)
            ;
        outb(COM1, text[i]);
    }
}

int main(int argc, char **argv)
{
    // serial_write_text((char *)"Hello, world!\n");

    // syscall(_DebugMessage, 1, "Hello, world!", "empty", 25, "empty");
    asm volatile("int $0x1");
    return 0;
}
