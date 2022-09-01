#include <filesystem.h>

#include <display.h>

#include "../drivers/disk.h"
#include "../drivers/serial.h"

namespace FileSystem
{
    ReadFSFunction(FB_Read)
    {
        uint32_t ret = CurrentDisplay->GetPixel(Offset, Size);
        Buffer = (uint8_t *)ret;
        return ret;
    }

    WriteFSFunction(FB_Write)
    {
        CurrentDisplay->SetPixel(Offset, Size, (uint32_t)((uint64_t)Buffer));
        return Size;
    }

    FileSystemOpeations fb = {
        .Name = "Framebuffer",
        .Read = FB_Read,
        .Write = FB_Write};

    FB::FB() { mountfs->MountFileSystem(&fb, 0666, "fb"); }

    FB::~FB() { warn("Destroyed"); }
}
