void *memcpy(void *__restrict__ dest, const void *__restrict__ src, unsigned long n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *c = (const unsigned char *)src;
    for (unsigned long i = 0; i < n; i++)
        d[i] = c[i];
    return dest;
}

void *memset(void *dest, int data, unsigned long nbytes)
{
    unsigned char *buf = (unsigned char *)dest;
    for (unsigned long i = 0; i < nbytes; i++)
        buf[i] = (unsigned char)data;
    return dest;
}

void *memmove(void *dest, const void *src, unsigned long n)
{
    unsigned char *dst = (unsigned char *)dest;
    const unsigned char *srcc = (const unsigned char *)src;
    if (dst < srcc)
    {
        for (unsigned long i = 0; i < n; i++)
            dst[i] = srcc[i];
    }
    else
    {
        for (unsigned long i = n; i != 0; i--)
            dst[i - 1] = srcc[i - 1];
    }
    return dest;
}

int memcmp(const void *vl, const void *vr, unsigned long n)
{
    const unsigned char *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++)
        ;
    return n ? *l - *r : 0;
}
