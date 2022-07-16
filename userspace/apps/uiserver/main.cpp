#include <syscalls.h>

enum MouseButton
{
    MouseNone,
    Right,
    Middle,
    Left
};

struct MouseInfo
{
    MouseButton Buttons;
    uint32_t X;
    uint32_t Y;
};

MouseInfo ReadMouseBuffer()
{
    File *mousefile = (File *)syscall_FileOpen((char *)"/dev/mouse");
    MouseInfo info = {MouseNone, 0, 0};
    syscall_FileRead(mousefile, 0, &info, sizeof(MouseInfo));
    syscall_FileClose(mousefile);
    return info;
}

void EventListener()
{
    while (true)
    {
    }
}

int main(int argc, char **argv)
{
    uint64_t address = syscall_displayAddress();
    uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();

    for (int VerticalScanline = 0; VerticalScanline < height; VerticalScanline++)
    {
        uint64_t PixelPtrBase = address + ((ppsl * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (ppsl * 4)); PixelPtr++)
            *PixelPtr = 0xFF000000;
    }
    syscall_createThread((long)EventListener, 0, 0);
    syscall_createProcess((char *)"/system/wm", 0, 0);
    while (1)
        ;
    return 0;
}
