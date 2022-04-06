#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n);
    void *memset(void *dest, int data, size_t nbytes);
    void *memmove(void *dest, const void *src, size_t n);
    int memcmp(const void *vl, const void *vr, size_t n);
    long unsigned strlen(char s[]);
    char *strcat(char *destination, const char *source);
    int strcmp(const char *l, const char *r);
    char *strstr(const char *haystack, const char *needle);
    
#ifdef __cplusplus
}
#endif
