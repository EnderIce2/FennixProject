#include <syscalls.h>
#include <task.h>

#include <msg.h>

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

enum UIServerMsgType
{
    UIServerMsgNone,
    UIServerMsgMouse,
    UIServerMsgKeyboard,
};

struct UIServerMsg
{
    UIServerMsgType Type;
    union
    {
        MouseInfo Mouse;
        char Key;
    } Data;
};

MouseInfo ReadMouseBuffer()
{
    void *mousefile = (void *)syscall_FileOpen("/dev/mouse");
    MouseInfo info = {MouseNone, 0, 0};
    syscall_FileRead(mousefile, 0, &info, sizeof(MouseInfo));
    syscall_FileClose(mousefile);
    return info;
}

void EventListener()
{
    syscall_createMessageListener("uiserver");

    while (true)
    {
        MessageQueue *queue = (MessageQueue *)syscall_getMessageQueue();
        for (uint64_t i = 0; i < MAX_MESSAGES; i++)
        {
            if (queue->Messages[i].Valid == false)
                continue;

            UIServerMsg *msg = (UIServerMsg *)queue->Messages[i].Buffer;

            switch (msg->Type)
            {
            case UIServerMsgMouse:
            {
                MouseInfo MsgData = ReadMouseBuffer();
                syscall_sendMessageByTID(queue->Messages[i].SourceTID, (void *)(&MsgData));
                break;
            }
            default:
                syscall_dbg(0x3F8, (char *)"UIServer: Unknown message received\n");
                break;
            }

            syscall_removeMessage(i);
        }
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
