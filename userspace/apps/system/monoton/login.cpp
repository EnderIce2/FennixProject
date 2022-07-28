#include "monoton.hpp"

static char usrbuf[1024];
static char pwdbuf[1024];

char *usr() { return usrbuf; }
char *pwd() { return pwdbuf; }

void clearbuffer()
{
    memset(usrbuf, 0, sizeof(usrbuf));
    memset(pwdbuf, 0, sizeof(pwdbuf));
}

int HasIllegalCharacter(char buf[])
{
    uint32_t curlen = 0;

    while (buf[curlen] != '\0')
    {
        switch (buf[curlen])
        {
        case ' ':
            return ' ';
        case '!':
            return '!';
        case '@':
            return '@';
        case '#':
            return '#';
        case '$':
            return '$';
        case '%':
            return '%';
        case '^':
            return '^';
        case '&':
            return '&';
        case '*':
            return '*';
        case '(':
            return '(';
        case ')':
            return ')';
        case '_':
            return '_';
        case '+':
            return '+';
        case '`':
            return '`';
        case '~':
            return '~';
        case '|':
            return '|';
        case '/':
            return '/';
        case '\\':
            return '\\';
        case '\"':
            return '\"';
        case '\'':
            return '\'';
        case '<':
            return '<';
        case '>':
            return '>';
        case '?':
            return '?';
        case '-':
            return '-';
        case '=':
            return '=';
        case ';':
            return ';';
        case ':':
            return ':';
        case '{':
            return '{';
        case '}':
            return '}';
        case '[':
            return '[';
        case ']':
            return ']';
        }
        curlen++;
    }
    return 0;
}

void InitLogin()
{
    int backspacelimit = 0;

RetryLoginName:
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

    if (isempty_1(usrbuf))
    {
        mono->print("\nThe username cannot be empty.\n");
        clearbuffer();
        goto RetryLoginName;
    }

    int illegal = HasIllegalCharacter(usrbuf);
    if (illegal != 0)
    {
        mono->print("\nThe username cannot contain \"");
        mono->printchar(illegal);
        mono->print("\" characters.\n");
        clearbuffer();
        goto RetryLoginName;
    }

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
