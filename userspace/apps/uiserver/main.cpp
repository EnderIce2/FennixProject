#include <syscalls.h>

int main(int argc, char **argv)
{
    uint64_t address = syscall_displayAddress();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();
    for (int VerticalScanline = 0; VerticalScanline < syscall_displayHeight(); VerticalScanline++)
    {
        uint64_t PixelPtrBase = address + ((ppsl * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (ppsl * 4)); PixelPtr++)
            *PixelPtr = 0xFF000000;
    }
    return 0;
}
