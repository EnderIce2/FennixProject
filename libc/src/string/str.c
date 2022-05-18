
long unsigned strlen(char s[])
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
