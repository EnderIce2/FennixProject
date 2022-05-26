#include <string.h>
#include <alloc.h>

#include <ctype.h>

long unsigned strlen(const char s[])
{
    long unsigned i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

int strncmp(const char *s1, const char *s2, unsigned long n)
{
    for (unsigned long i = 0; i < n; i++)
    {
        char c1 = s1[i], c2 = s2[i];
        if (c1 != c2)
            return c1 - c2;
        if (!c1)
            return 0;
    }
    return 0;
}

char *strcat(char *destination, const char *source)
{
    if ((destination == (void *)0) && (source == (void *)0))
        return (void *)0;
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
    if (destination == (void *)0)
        return (void *)0;
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
    if (destination == (void *)0)
        return (void *)0;
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
            return (void *)0;
        if (*a++ != *b++)
        {
            a = ++haystack;
            b = needle;
        }
    }
}

char *strdup(const char *String)
{
    char *OutBuffer = malloc(strlen((char *)String) + 1);
    strcpy(OutBuffer, String);
    return OutBuffer;
}

char *strchr(const char *String, int Char)
{
    while (*String != (char)Char)
    {
        if (!*String++)
            return 0;
    }
    return (char *)String;
}

char *strrchr(const char *String, int Char)
{
    char *ret = 0;
    do
    {
        if (*String == (char)Char)
            ret = (char *)String;
    } while (*String++);
    return ret;
}

int strncasecmp(const char *lhs, const char *rhs, long unsigned int Count)
{
    while (Count--)
    {
        if (*lhs == 0 || *rhs == 0)
        {
            return *(unsigned char *)(lhs) - *(unsigned char *)(rhs);
        }

        if (tolower(*lhs) != tolower(*rhs))
        {
            return *(unsigned char *)(lhs) - *(unsigned char *)(rhs);
        }
        lhs++;
        rhs++;
    }
    return 0;
}

int strcasecmp(const char *lhs, const char *rhs)
{
    while (*lhs && (tolower(*lhs) == tolower(*rhs)))
    {
        lhs++;
        rhs++;
    }

    int lc = tolower(*lhs);
    int rc = tolower(*rhs);
    return lc - rc;
}

int isdigit(int Char)
{
    return Char >= '0' && Char <= '9';
}

int isspace(int Char)
{
    return Char == ' ' || Char == '\t' || Char == '\r' || Char == '\n' || Char == '\f' || Char == '\v';
}