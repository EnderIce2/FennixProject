#include <print.h>

#include "../printf.h"

#include <string.h>
#include <system.h>
#include <syscalls.h>

FILE *fslin;
FILE *fslout;
FILE *fslerr;

static inline void append_printf(char s[], char n)
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;
    int len = i;
    s[len] = n;
    s[len + 1] = '\0';
}

static inline void syswritedbgprint_wrapper(char c, void *unused)
{
    append_printf(fslout->PrintBuffer, c);
    (void)unused;
}

int fprintf(FILE *Stream, const char *Format, ...)
{
    WriteSysDebugger("fprintf( %p %s ) stub\n", Stream, Format);
    return 0;
}

int printf(const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    int ret = vfctprintf(syswritedbgprint_wrapper, 0, Format, Args);
    va_end(Args);
    return ret;
}

int vfprintf(FILE *Stream, const char *Format, va_list Args)
{
    return vfctprintf(syswritedbgprint_wrapper, 0, Format, Args);
}

int vprintf(const char *Format, va_list Args)
{
    return vfctprintf(syswritedbgprint_wrapper, 0, Format, Args);
}

int snprintf(char *String, size_t Length, const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    int ret = snprintf_(String, Length, Format, Args);
    va_end(Args);
    return ret;
}

int vsnprintf(char *String, size_t Length, const char *Format, va_list Args)
{
    return vsnprintf_(String, Length, Format, Args);
}

int vsscanf(const char *String, const char *Format, va_list Args)
{
    int count = 0;
    for (;;)
    {
        char c = *Format++;
        if (!c)
            break;

        if (isspace(c))
            while (isspace(*String))
                ++String;
        else if (c != '%')
        {
        match:
            if (*String == '\0')
                goto end;
            if (*String != c)
                goto fail;
            ++String;
        }
        else
        {
            c = *Format++;
            char type = c;
            switch (type)
            {
            case '%':
                goto match;
            case 'd':
            {
                int sign = 1;
                c = *String++;
                if (c == '\0')
                    goto end;
                if (c == '-')
                {
                    sign = -1;
                    c = *String++;
                }
                int n = 0;
                while (isdigit(c))
                {
                    n = n * 10 + c - '0';
                    c = *String++;
                }
                n *= sign;
                --String;
                int *result = va_arg(Args, int *);
                *result = n;
                ++count;
            }
            break;
            }
        }
    }
fail:
    return count;
end:
    return count ? count : -1;
}

int sscanf(const char *String, const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    int ret = vsscanf(String, Format, Args);
    va_end(Args);
    return ret;
}

int fputs(const char *String, FILE *Stream)
{
    size_t i = 0;
    for (i = 0; i < strlen((char *)String); i++)
        append_printf(Stream->PrintBuffer, String[i]);
    return i;
}

int puts(const char *String)
{
    int ret = fputs(String, fslout);
    append_printf(fslout->PrintBuffer, '\n');
    return ret;
}

int fputc(int Char, FILE *Stream)
{
    append_printf(Stream->PrintBuffer, Char);
    return Char;
}

int putchar(int Char)
{
    append_printf(fslout->PrintBuffer, Char);
    return 0;
}

int fflush(FILE *stream)
{
    WriteSysDebugger("fflush( %p ) stub\n", stream);
    syscall_dbg(0x3F8, (char *)"\nfslout: ");
    syscall_dbg(0x3F8, (char *)fslout->PrintBuffer);
    syscall_dbg(0x3F8, (char *)"\nfslerr: ");
    syscall_dbg(0x3F8, (char *)fslerr->PrintBuffer);
    syscall_dbg(0x3F8, (char *)"\n");
    fslout->PrintBuffer[0] = '\0';
    fslerr->PrintBuffer[0] = '\0';
    return 0;
}
