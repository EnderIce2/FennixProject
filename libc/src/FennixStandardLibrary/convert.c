#include <convert.h>

#include <stdint.h>
#include <string.h>

int atoi(const char *String)
{
    uint64_t Length = strlen((char *)String);
    uint64_t OutBuffer = 0;
    uint64_t Power = 1;
    for (uint64_t i = Length; i > 0; --i)
    {
        OutBuffer += (String[i - 1] - 48) * Power;
        Power *= 10;
    }
    return OutBuffer;
}

// from https://github.com/GaloisInc/minlibc/blob/master/atof.c
double atof(const char *String)
{
    double a = 0.0;
    int e = 0;
    int c;
    while ((c = *String++) != '\0' && isdigit(c))
    {
        a = a * 10.0 + (c - '0');
    }
    if (c == '.')
    {
        while ((c = *String++) != '\0' && isdigit(c))
        {
            a = a * 10.0 + (c - '0');
            e = e - 1;
        }
    }
    if (c == 'e' || c == 'E')
    {
        int sign = 1;
        int i = 0;
        c = *String++;
        if (c == '+')
            c = *String++;
        else if (c == '-')
        {
            c = *String++;
            sign = -1;
        }
        while (isdigit(c))
        {
            i = i * 10 + (c - '0');
            c = *String++;
        }
        e += i * sign;
    }
    while (e > 0)
    {
        a *= 10.0;
        e--;
    }
    while (e < 0)
    {
        a *= 0.1;
        e++;
    }
    return a;
}
