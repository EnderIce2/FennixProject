#include <heap.h>
#include "../boot/stivale2.h"
#include "../kernel.h"

using namespace PMM;
using namespace VMM;

void init_pmm()
{
    trace("initializing pmm");
    uint64_t kernelSize = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    // uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    uint64_t fbBase = earlyparams.Framebuffer.BaseAddress;
    uint64_t fbSize = earlyparams.Framebuffer.BufferSize + 0x1000;
    KernelAllocator = PageFrameAllocator();
    KernelAllocator.ReadMemoryMap();
    // KernelAllocator.LockPages(&_kernel_start, kernelPages);

    debug("RAM SIZE: %dGB", TO_GB(earlyparams.mem.Size));
    debug("Kernel Start: %016p - End: %016p - Size: %d (%dKB)", &_kernel_start, &_kernel_end, kernelSize, TO_KB(kernelSize));
    debug("Framebuffer Base: %016p - Framebuffer Size: %d (%dKB)", fbBase, fbSize, TO_KB(fbSize));
    debug("RAM-> Free: %dGB (%dMB) - Used: %dGB (%dMB) - Reserved: %dGB (%dMB)",
          TO_GB(GetFreeMemory()), TO_MB(GetFreeMemory()),
          TO_GB(GetUsedMemory()), TO_MB(GetUsedMemory()),
          TO_GB(GetReservedMemory()), TO_MB(GetReservedMemory()));
}
