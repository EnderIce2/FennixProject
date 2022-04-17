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
    int strncmp(const char *s1, const char *s2, long unsigned int n);
    long unsigned strlen(char s[]);
    char *strcat(char *destination, const char *source);
    char *strcpy(char *destination, const char *source);
    char *strncpy(char *destination, const char *source, unsigned long num);
    int strcmp(const char *l, const char *r);
    char *strstr(const char *haystack, const char *needle);
    int isdigit(char c);
    int isspace(char c);
    int isempty(char *str);
    
#ifdef __cplusplus
}
#endif
