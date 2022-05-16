#include "loadpsf.h"
#include <syscalls.h>

int main(int argc, char **argv)
{
    syscall_dbg(0x3F8, (char *)"[MonotonShell] Started.\n");
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 2)
    {
        syscall_dbg(0x3F8, (char *)"[MonotonShell] Error! Program launched from multitasking mode.\n");
        while (1)
            ;
    }

    char *fonts0 = "/system/fonts/tamsyn-font-1.11/Tamsyn5x9b.psf";
    char *fonts1 = "/system/fonts/tamsyn-font-1.11/Tamsyn5x9r.psf";
    char *fonts2 = "/system/fonts/tamsyn-font-1.11/Tamsyn6x12b.psf";
    char *fonts3 = "/system/fonts/tamsyn-font-1.11/Tamsyn6x12r.psf";
    char *fonts4 = "/system/fonts/tamsyn-font-1.11/Tamsyn7x13b.psf";
    char *fonts5 = "/system/fonts/tamsyn-font-1.11/Tamsyn7x13r.psf";
    char *fonts6 = "/system/fonts/tamsyn-font-1.11/Tamsyn7x14b.psf";
    char *fonts7 = "/system/fonts/tamsyn-font-1.11/Tamsyn7x14r.psf";
    char *fonts8 = "/system/fonts/tamsyn-font-1.11/Tamsyn8x15b.psf";
    char *fonts9 = "/system/fonts/tamsyn-font-1.11/Tamsyn8x15r.psf";
    char *fonts10 = "/system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf";
    char *fonts11 = "/system/fonts/tamsyn-font-1.11/Tamsyn8x16r.psf";
    char *fonts12 = "/system/fonts/tamsyn-font-1.11/Tamsyn10x20b.psf";
    char *fonts22 = "/system/fonts/tamsyn-font-1.11/Tamsyn10x20r.psf";

    // Default font: /system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf
    int times = 0;
    while (1)
    {
        syscall_getLastKeyboardScanCode();
        syscall_getLastKeyboardScanCode();
        switch (times)
        {
        case 0:
            LoadFont(fonts0);
            break;
        case 1:
            LoadFont(fonts1);
            break;
        case 2:
            LoadFont(fonts2);
            break;
        case 3:
            LoadFont(fonts3);
            break;
        case 4:
            LoadFont(fonts4);
            break;
        case 5:
            LoadFont(fonts5);
            break;
        case 6:
            LoadFont(fonts6);
            break;
        case 7:
            LoadFont(fonts7);
            break;
        case 8:
            LoadFont(fonts8);
            break;
        case 9:
            LoadFont(fonts9);
            break;
        case 10:
            LoadFont(fonts10);
            break;
        case 11:
            LoadFont(fonts11);
            break;
        case 12:
            LoadFont(fonts12);
            break;
        case 13:
            LoadFont(fonts22);
            break;
        default:
            break;
        }
        times++;
    }
    return 1;
}
