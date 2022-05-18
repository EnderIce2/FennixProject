#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    const char sc_ascii_low[] = {'?', '?', '1', '2', '3', '4', '5', '6',
                                 '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                                 'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                                 'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                                 'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

    const char sc_ascii_high[] = {'?', '?', '!', '@', '#', '$', '%', '^',
                                  '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
                                  'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G',
                                  'H', 'J', 'K', 'L', ';', '\"', '~', '?', '|', 'Z', 'X', 'C', 'V',
                                  'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};

    enum SpecialKeys
    {
        KEY_INVALID = 0xFEFFF,
        KEY_BACKSPACE = 0xFF000,
        KEY_SPACE = 0xFF001,
        KEY_ENTER = 0xFF002,
        KEY_ESC = 0xFF003,
        KEY_TAB = 0xFF004,
        KEY_SHIFT = 0xFF005,
        KEY_CTRL = 0xFF006,
        KEY_ALT = 0xFF007,
        KEY_CAPS = 0xFF008,
        KEY_F1 = 0xFF009,
        KEY_F2 = 0xFF00A,
        KEY_F3 = 0xFF00B,
        KEY_F4 = 0xFF00C,
        KEY_F5 = 0xFF00D,
        KEY_F6 = 0xFF00E,
        KEY_F7 = 0xFF00F,
        KEY_F8 = 0xFF010,
        KEY_F9 = 0xFF011,
        KEY_F10 = 0xFF012,
        KEY_F11 = 0xFF013,
        KEY_F12 = 0xFF014,
        KEY_UP = 0xFF015,
        KEY_DOWN = 0xFF016,
        KEY_LEFT = 0xFF017,
        KEY_RIGHT = 0xFF018,
        KEY_INSERT = 0xFF019,
        KEY_DELETE = 0xFF01A,
        KEY_HOME = 0xFF01B,
        KEY_END = 0xFF01C,
        KEY_PAGEUP = 0xFF01D,
        KEY_PAGEDOWN = 0xFF01E,
        KEY_NUMLOCK = 0xFF01F,
        KEY_SCROLLLOCK = 0xFF020,
        KEY_PRINTSCREEN = 0xFF021,
        KEY_PAUSE = 0xFF022,
        KEY_BREAK = 0xFF023,
    };

    uint32_t GetLetterFromScanCode(uint8_t ScanCode);

#ifdef __cplusplus
}
#endif
