#include "sse.h"
#include <asm.h>
#include "cpuid.h"

void enable_sse()
{
    if (cpu_feature(CPUID_FEAT_RDX_SSE))
    {
        debug("Enabling SSE support...");
        CR0 cr0 = readcr0();
        CR4 cr4 = readcr4();
        cr0.EM = 0;
        cr0.MP = 1;
        cr4.OSFXSR = 1;
        cr4.OSXMMEXCPT = 1;
        writecr0(cr0);
        writecr4(cr4);
        // enable_sse_asm();

        // EM &= ~(1 << 2);
        // MP |= (1 << 1);
        // OSFXSR |= (1 << 9);
        // OSXMMEXCPT |= (1 << 10);
    }
}
