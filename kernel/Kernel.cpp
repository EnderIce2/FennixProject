#include "kernel.h"

extern "C" void kernel_entry(void *data)
{
    while (1)
        asm volatile("hlt");
}
