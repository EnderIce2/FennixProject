#include "kernel.h"

extern "C" void kernel_entry(void *data)
{
    while (1)
        __asm__ volatile("hlt");
}
