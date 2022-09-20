#include "kernel.h"

#ifdef __cplusplus

#define EXTERNC extern "C"
#define START_EXTERNC \
    extern "C"        \
    {
#define END_EXTERNC \
    }

#else

#define EXTERNC
#define START_EXTERNC
#define END_EXTERNC

#endif

#define trace
#define warn
#define err
#define CPU_HALT

void KernelInit();

EXTERNC void kernel_entry(void *Data)
{
    err("Bootloader initialized the kernel with unknown protocol!");
    err("System Halted.");
    CPU_HALT;
}

EXTERNC void kernel_main_aarch64()
{
}

void KernelTask()
{
}

void KernelInit()
{
    trace("Early initialization completed.");
}
