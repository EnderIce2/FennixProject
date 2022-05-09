#include <syscalls.h>
#include <task.h>

// https://wiki.osdev.org/GUI

void PutRect(int X, int Y, int Width, int Height, int RGB) {}
void PutLine(int X1, int Y1, int X2, int Y2, int RGB) {}
void memcpy(uint64_t Dest, uint64_t Source, int Size) {}

void *alloc(uint64_t size) {}

#define rgbWhite 0xFFFFFF

struct vbe
{
    uint64_t XResolution;
    uint64_t YResolution;
    uint64_t BitsPerPixel;
    uint64_t VideoBaseAddr;
    uint64_t BytesPerScanLine;
};

struct ms
{
    int X, Y;
};

struct scren
{
    int Width, Height;
};

ms Mouse;
vbe VbeModeInfoBlock;
scren Screen;

uint64_t ScreenZBuffer;
uint64_t ScreenBufferSize;
void *ScreenBackBuffer;

char g_ScreenBuffer[9999];

void PaintDesktop()
{
    // fill the background of the desktop
    PutRect(0, 0, Screen.Width, Screen.Height, 0xc0c0c0c0);
    // now tell every child window to paint itself:
    // for (int i = 0; i < ChildWindows.size(); i++)
    // {
    //     ChildWindows[i].Paint();
    // }
}

void PaintCursor()
{
    // just make a white box at the cursor position:
    PutRect(Mouse.X, Mouse.Y, 5, 5, rgbWhite);
}

void ScreenRefreshProc()
{
    syscall_dbg(0x3F8, (char *)"WM: Screen Refresh Started\n");

    for (;;)
    {
        PaintDesktop(); // paints the desktop and all windows
        PaintCursor();  // finally paints the cursor so it's on top
#if WAIT_FOR_VERTICAL_RETRACE
        while ((inportb(0x3DA) & 0x08))
            ;
        while (!(inportb(0x3DA) & 0x08))
            ;
#endif
        memcpy(VbeModeInfoBlock.VideoBaseAddr, ScreenZBuffer, ScreenBufferSize);
    }
}

void PutPixel(int x, int y, int color)
{
    // do not write memory outside the screen buffer, check parameters against the VBE mode info
    if (x < 0 || x > VbeModeInfoBlock.XResolution || y < 0 || y > VbeModeInfoBlock.YResolution)
        return;
    if (x)
        x = (x * (VbeModeInfoBlock.BitsPerPixel >> 3));
    if (y)
        y = (y * VbeModeInfoBlock.BytesPerScanLine);
    char *cTemp;
    cTemp = &g_ScreenBuffer[x + y];
    cTemp[0] = color & 0xff;
    cTemp[1] = (color >> 8) & 0xff;
    cTemp[2] = (color >> 16) & 0xff;
}

// void Window::Paint()
// {
//     // paint a navy blue window
//     PutRect(this->Left, this->Top, this->Width, this->Height, rgbNavy);
//     // put a small red square in the top right hand corner of the window
//     PutRect(this->Left + this->Width - 5, this->Top, 5, 5, rgbRed);
//     // put the title bar text at the top of the window
//     Put8x8String(this->Left + 1, this->Top + 1, this->Title, rgbBlack);
//     for (int i = 0; i < ChildWindows.size(); i++)
//     {
//         ChildWindows[i].Paint();
//     }
// }

void InitScreen()
{
    ScreenBufferSize = VbeModeInfoBlock.XResolution * VbeModeInfoBlock.YResolution * (VbeModeInfoBlock.BitsPerPixel / 8);
    ScreenBackBuffer = alloc(ScreenBufferSize);
    syscall_createThread((uint64_t)ScreenRefreshProc, 0, 0);
}

int main(int argc, char **argv)
{
    PutRect(0, 0, Screen.Width, Screen.Height, 0x282828);
    /* ... show login screen and wait for user input ... */
    InitScreen();
    syscall_createProcess((char *)"/system/fesktop", 0, 0);
    while (1)
        ;
    return 0;
}
