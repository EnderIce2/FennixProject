#pragma once
#include "../../../libs/monoton/monotonlib.h"
#include "../../../libs/scparse/scparse.h"
#include <string.h>
#include <syscalls.h>

static inline void backspace(char s[])
{
    int len = strlen(s);
    s[len - 1] = '\0';
}

static inline void append(char s[], char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

static inline int isempty_1(char *str)
{
    long unsigned i = 0;
    while (str[i] != '\0')
        ++i;

    if (i == 0)
        return 1;

    while (*str != '\0')
    {
        if (!isspace(*str))
            return 0;
        str++;
    }
    return 1;
}

extern MonotonLib::mtl *mono;

char *usr();
char *pwd();
void InitLogin();
