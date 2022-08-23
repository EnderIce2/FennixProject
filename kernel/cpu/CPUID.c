#include "cpuid.h"
#include <asm.h>
#include <string.h>
#include <heap.h>

bool cpu_feature(enum CPU_FEATURE feature)
{
    uint32_t rax, rbx, rcx, rdx;
    cpuid(0x01, &rax, &rbx, &rcx, &rdx);
    if (rdx & feature || rcx & feature)
        return true;
    return false;
}
