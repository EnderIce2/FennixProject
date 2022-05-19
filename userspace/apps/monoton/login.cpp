#include "monoton.hpp"

static char usrbuf[1024];
static char pwdbuf[1024];

char *usr() { return usrbuf; }
char *pwd() { return pwdbuf; }

void InitLogin()
{
    int backspacelimit = 0;

    mono->print("Login as: ");
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
                    backspace(usrbuf);
                    backspacelimit--;
                }
            }
            else if (key == '\n')
            {
                break;
            }
            else
            {
                append(usrbuf, key);
                mono->printchar(key);
                backspacelimit++;
            }
        }
    }

    backspacelimit = 0;

    mono->print("\nPassword for ");
    mono->print(usrbuf);
    mono->print(": ");

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
                    backspace(pwdbuf);
                    backspacelimit--;
                }
            }
            else if (key == '\n')
            {
                break;
            }
            else
            {
                append(pwdbuf, key);
                mono->printchar('*');
                backspacelimit++;
            }
        }
    }
}
