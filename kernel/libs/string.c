#include <string.h>
#include <stdint.h>
#include <stddef.h>

void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *c = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++)
        d[i] = c[i];
    return dest;
}

void *memset(void *dest, int data, size_t nbytes)
{
    unsigned char *buf = (unsigned char *)dest;
    for (size_t i = 0; i < nbytes; i++)
        buf[i] = (unsigned char)data;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *dst = (unsigned char *)dest;
    const unsigned char *srcc = (const unsigned char *)src;
    if (dst < srcc)
    {
        for (size_t i = 0; i < n; i++)
            dst[i] = srcc[i];
    }
    else
    {
        for (size_t i = n; i != 0; i--)
            dst[i - 1] = srcc[i - 1];
    }
    return dest;
}

int memcmp(const void *vl, const void *vr, size_t n)
{
    const unsigned char *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++)
        ;
    return n ? *l - *r : 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        char c1 = s1[i], c2 = s2[i];
        if (c1 != c2)
            return c1 - c2;
        if (!c1)
            return 0;
    }
    return 0;
}

long unsigned strlen(char s[])
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

char *strcat(char *destination, const char *source)
{
    if ((destination == NULL) && (source == NULL))
        return NULL;
    char *start = destination;
    while (*start != '\0')
    {
        start++;
    }
    while (*source != '\0')
    {
        *start++ = *source++;
    }
    *start = '\0';
    return destination;
}

char *strcpy(char *destination, const char *source)
{
    if (destination == NULL)
        return NULL;
    char *ptr = destination;
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char *strncpy(char *destination, const char *source, unsigned long num)
{
    if (destination == NULL)
        return NULL;
    char *ptr = destination;
    while (*source && num--)
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

int strcmp(const char *l, const char *r)
{
    for (; *l == *r && *l; l++, r++)
        ;
    return *(unsigned char *)l - *(unsigned char *)r;
}

char *strstr(const char *haystack, const char *needle)
{
    const char *a = haystack, *b = needle;
    while (1)
    {
        if (!*b)
            return (char *)haystack;
        if (!*a)
            return NULL;
        if (*a++ != *b++)
        {
            a = ++haystack;
            b = needle;
        }
    }
}

int isdigit(char c)
{
    return c >= '0' && c <= '9';
}

int isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}

int isempty(char *str)
{
    if (strlen(str) == 0)
        return 1;
    while (*str != '\0')
    {
        if (!isspace(*str))
            return 0;
        str++;
    }
    return 1;
}
