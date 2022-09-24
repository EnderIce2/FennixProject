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
    File *mousefile = (File *)syscall_FileOpen((char *)"/system/dev/mouse");
    MouseInfo info = {MouseNone, 0, 0};
    if (mousefile->Status == FileStatus::OK)
        syscall_FileRead(mousefile, 0, (uint8_t *)&info, sizeof(MouseInfo));
    syscall_FileClose(mousefile);
    return info;
}

void EventListener()
{
    while (true)
    {
    }
}

int main(int argc, char *argv[])
{
    syscall_createThread((long)EventListener, 0, 0);
    syscall_createProcess((char *)"/system/wm", 0, 0);
    while (1)
        ;
    return 0;
}
