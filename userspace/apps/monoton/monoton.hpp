#pragma once
#include "../../libs/monoton/monotonlib.h"
#include "../../libs/scparse/scparse.h"
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

extern MonotonLib::mtl *mono;

char *usr();
char *pwd();
void InitLogin();
