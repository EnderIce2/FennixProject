#include "../../libs/monoton/monotonlib.h"
#include "../../libs/scparse/scparse.h"
#include <string.h>
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

    char *fonts0 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn5x9b.psf";
    char *fonts1 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn5x9r.psf";
    char *fonts2 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn6x12b.psf";
    char *fonts3 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn6x12r.psf";
    char *fonts4 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn7x13b.psf";
    char *fonts5 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn7x13r.psf";
    char *fonts6 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn7x14b.psf";
    char *fonts7 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn7x14r.psf";
    char *fonts8 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x15b.psf";
    char *fonts9 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x15r.psf";
    char *fonts10 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf";
    char *fonts11 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x16r.psf";
    char *fonts12 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn10x20b.psf";
    char *fonts22 = (char *)"/system/fonts/tamsyn-font-1.11/Tamsyn10x20r.psf";

    uint64_t address = syscall_displayAddress();
    // uint64_t width = syscall_displayWidth();
    // uint64_t height = syscall_displayHeight();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();
    for (int VerticalScanline = 0; VerticalScanline < syscall_displayHeight(); VerticalScanline++)
    {
        uint64_t PixelPtrBase = address + ((ppsl * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (ppsl * 4)); PixelPtr++)
            *PixelPtr = 0xFF000000;
    }

    // Default font: /system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf
    MonotonLib::mtl *mono = new MonotonLib::mtl((char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf");
    mono->print("MonotonShell (Testing)\n> ");

    while (1)
    {
        uint8_t scancode = (uint8_t)syscall_getLastKeyboardScanCode();
        uint32_t key = GetLetterFromScanCode(scancode); // down

        // test code
        static bool testcodeshift = false;
        if (scancode & 0x80)
            if (scancode == 0xaa || scancode == 0xb6)
                testcodeshift = false;

        if (scancode <= 57)
        {
            if (scancode == 0x2a ||
                scancode == 0x36 ||
                scancode == 0x38 ||
                scancode == 0xB8)
                testcodeshift = true;
            else
            {
                static char res[] = {'\0', '\0'};
                if (scancode == 0x1C) // enter
                    res[0] = '\n';
                else
                {
                    if (!testcodeshift)
                    {
                        res[0] = sc_ascii_low[scancode];
                    }
                    else
                    {
                        res[0] = sc_ascii_high[scancode];
                    }
                }
                syscall_dbg(0x3F8, (char *)(res));
                mono->print((char *)(res));
            }
        }

        // if (key < KEY_INVALID)
        // {
        // syscall_dbg(0x3F8, (char *)(key));
        // mono->print((char *)(key));
        // }
        // else
        // {
        //     if (key == KEY_INVALID)
        //         syscall_dbg(0x3F8, (char *)"INVALID KEY.\n");
        //     else
        //         syscall_dbg(0x3F8, (char *)"Other key pressed.\n");
        // }
        // scancode = syscall_getLastKeyboardScanCode();
        // GetLetterFromScanCode(scancode); // up ---- call this one more time to get shift key
    }
    return 1;
}
