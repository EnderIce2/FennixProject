#include "monoton.hpp"

#include <system.h>

MonotonLib::mtl *mono = nullptr;
static char key_buffer[1024];

void ParseBuffer(char *Buffer)
{
    mono->printchar('\n');
    if (strcmp(Buffer, "help") == 0)
    {
        mono->print("Monoton Shell for Fennix\n");
        mono->print("help          - Show this screen\n");
        mono->print("clear         - Clear screen\n");
    }
    else if (strcmp(Buffer, "clear") == 0)
    {
        mono->Clear();
    }
    else if (strcmp(Buffer, "doom") == 0)
    {
        syscall_createProcess((char *)"/system/doom", 0, 0);
        syscall_pushTask();
    }
    else
    {
        mono->print(Buffer);
        mono->print(": Command not found.");
    }
    mono->printchar('\n');
    mono->print(usr());
    mono->print((char *)"@fennix:/$ ");
    File *f = (File *)syscall_FileOpen("/system/doom");
    mono->print(f->Name);
}

int main(int argc, char **argv)
{
    WriteSysDebugger("[MonotonShell] Started.\n");
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 2)
    {
        WriteSysDebugger("[MonotonShell] Error! Program launched from multitasking mode.\n");
        while (1)
            ;
    }

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
    mono = new MonotonLib::mtl((char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf");

    InitLogin();
    mono->print((char *)"\n-- This shell is not fully implemented! --\n");
    mono->print(usr());
    mono->print((char *)"@fennix:/$ ");
    int backspacelimit = 0;

    while (1)
    {
        int key = GetLetterFromScanCode((uint8_t)syscall_getLastKeyboardScanCode());

        if (key != KEY_INVALID)
        {
            if (key == KEY_D_BACKSPACE)
            {
                if (backspacelimit > 0)
                {
                    mono->RemoveChar();
                    backspace(key_buffer);
                    backspacelimit--;
                }
            }
            else if (key == '\n')
            {
                ParseBuffer(key_buffer);
                backspacelimit = 0;
                key_buffer[0] = '\0';
            }
            else
            {
                append(key_buffer, key);
                mono->printchar(key);
                backspacelimit++;
            }
        }
    }
    return 1;
}
