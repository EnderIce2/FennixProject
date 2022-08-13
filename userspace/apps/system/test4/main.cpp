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

int main(int argc, char *argv[])
{

    drawrectangle(60, 0, 20, 100, 0xFF00FF);
    while (1)
    {
    }
    return 0;
}
