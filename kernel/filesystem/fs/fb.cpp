#include <filesystem.h>

#include <display.h>

#include "../../drivers/disk.h"
#include "../../drivers/serial.h"

namespace FileSystem
{
    uint64_t fbAddress;
    uint64_t fbSize;
    uint32_t fbWidth, fbHeight;
    uint32_t fbPixelsPerScanLine;

    ReadFSFunction(FB_Read)
    {
        uint64_t X = Offset;
        uint64_t Y = Size;
        if (X >= fbWidth || Y >= fbHeight)
            Buffer = 0;
        Buffer = (uint8_t *)((uint64_t)(((uint32_t *)fbAddress)[X + (Y * fbPixelsPerScanLine)]));
        return fbSize;
    }

    WriteFSFunction(FB_Write)
    {
        uint64_t X = Offset;
        uint64_t Y = Size;
        if (X >= fbWidth || Y >= fbHeight)
            return 0;
        ((uint32_t *)fbAddress)[X + (Y * fbPixelsPerScanLine)] = (uint32_t)((uint64_t)Buffer);
        return fbSize;
    }

    FileSystemOpeations fb = {
        .Name = "Framebuffer",
        .Read = FB_Read,
        .Write = FB_Write};

    void FB::SetFrameBufferData(uint64_t Address, uint64_t Size, uint32_t Width, uint32_t Height, uint32_t PixelsPerScanLine)
    {
        fbAddress = Address;
        fbSize = Size;
        fbWidth = Width;
        fbHeight = Height;
        fbPixelsPerScanLine = PixelsPerScanLine;
    }

    FB::FB() { devfs->AddFileSystem(&fb, 0666, "fb", FileSystem::NodeFlags::FS_PIPE); }

    FB::~FB() { warn("Destroyed"); }
}
