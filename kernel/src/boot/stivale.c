#include "stivale.h"
#include "../kernel.h"

// The support is not going to be available any time soon, because it's not needed and it's a deprecated protocol.

static uint8_t stivale1_stack[4096];

// __attribute__((section(".stivalehdr"), used)) static struct stivale_header stivale_hdr = {
//     .stack = (uint64_t)stivale1_stack + sizeof(stivale1_stack),
//     .flags = (1 << 0) | (1 << 3),
//     .framebuffer_width = 0,
//     .framebuffer_height = 0,
//     .framebuffer_bpp = 0,
//     .entry_point = (uint64_t)stivale_initializator};

bool init_stivale(struct stivale_struct *bootloaderdata, GlobalBootParams *params)
{
    fixme("unimplemented");
    return false;
}

bool detect_stivale(void *data)
{
    fixme("unimplemented");
    return false;
}