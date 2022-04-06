#include "stivale.h"
#include "../kernel.h"

// The support is not going to be available any time soon, because it's not needed and it's a deprecated protocol.

/* It won't be called because the kernel virtual address is starting at 0xffffffff80000000 and not 0xffffffff82000000 */

__attribute__((section(".stivalehdr"), used)) static struct stivale_header stivale_hdr = {
    .stack = (uint64_t)kernel_stack + sizeof(kernel_stack),
    .flags = (1 << 0) | (1 << 3),
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
    .entry_point = (uint64_t)stivale_initializator};

bool init_stivale(struct stivale_struct *bootloaderdata, GlobalBootParams *params)
{
    for (int VerticalScanline = 0; VerticalScanline < bootloaderdata->framebuffer_height; VerticalScanline++)
    {
        uint64_t PixelPtrBase = bootloaderdata->framebuffer_addr + ((bootloaderdata->framebuffer_bpp * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (bootloaderdata->framebuffer_bpp * 4)); PixelPtr++)
            *PixelPtr = 0xFFFF0000;
    }
    fixme("Unimplemented");
    return false;
}

bool detect_stivale(void *data)
{
    fixme("Unimplemented");
    return false;
}