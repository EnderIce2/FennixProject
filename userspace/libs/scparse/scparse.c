#include "scparse.h"

static int scparse__shiftkey = 0;

uint32_t GetLetterFromScanCode(uint8_t ScanCode)
{
    return sc_ascii_low[ScanCode];
    if (ScanCode & 0x80)
    {
        if (ScanCode == 0xaa || ScanCode == 0xb6)
        {
            scparse__shiftkey = 0;
        }
    }
    switch (ScanCode)
    {
    case 0x1C:
        return KEY_ENTER;
    case 0x2a:
    case 0x36:
    {
        scparse__shiftkey = 1;
        return KEY_SHIFT;
    }
    case 0x0E:
        return KEY_BACKSPACE;
    case 0x48:
        return KEY_UP;
    case 0x50:
        return KEY_DOWN;
    case 0xf:
        return KEY_TAB;
    case 0x1D:
    case 0x9D:
        return KEY_CTRL;
    case 0x38:
    case 0xB8:
    {
        scparse__shiftkey = 1;
        return KEY_SHIFT;
    }
    default:
    {
        if (ScanCode > 57)
            break;
        static char res[] = {'\0', '\0'}; // this works
        if (!scparse__shiftkey)
        {
            res[0] = sc_ascii_low[ScanCode];
        }
        else
        {
            res[0] = sc_ascii_high[ScanCode];
        }
        return (uint32_t)res; // but conversion is not working properly
        // i should make something that works but i don't have time to do it now
    }
    }
    return KEY_INVALID;
}
