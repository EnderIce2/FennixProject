#include <syscalls.h>

void drawrectangle(uint64_t X, uint64_t Y, uint64_t W, uint64_t H, uint32_t C)
{
    uint64_t address = syscall_displayAddress();
    uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();
    for (int y = Y; y < Y + H; y++)
        for (int x = X; x < X + W; x++)
        {
            if (x >= width || y >= height)
                continue;
            ((uint32_t *)address)[x + (y * ppsl)] = C;
        }
}

void Desktop()
{
    uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();
    drawrectangle(0, 0, width, height - 20, 0x008080);

    while (1)
        ;
}

void Dock()
{
    uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();

    drawrectangle(0, height - 20, width, 20, 0xacacac);

    // drawrectangle(2, height - 18, 50, 16, 0x5c5c5c);
    // drawrectangle(3, height - 17, 49, 15, 0x8c8c8c);

    int color1 = 0x5c5c5c;
    int color2 = 0x8c8c8c;

    while (1)
    {
        if (color1 > 0xFFFFFF)
            color1 = 0x5c5c5c;
        if (color2 > 0xFFFFFF)
            color2 = 0x8c8c8c;
        
        drawrectangle(2, height - 18, 50, 16, color1);
        drawrectangle(3, height - 17, 49, 15, color2);

        color1 += 0x00000F;
        color2 += 0x00000F;
    }
}

int main(int argc, char **argv)
{
    syscall_createThread((long)Desktop, 0, 0);
    syscall_createThread((long)Dock, 0, 0);
    while (1)
        ;
    return 0;
}
