#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif
    int isdigit(int c);
    int isspace(int c);
    int isempty(char *str);
    unsigned int isdelim(char c, char *delim);

    void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n);
    void *memset(void *dest, int data, size_t nbytes);
    void *memmove(void *dest, const void *src, size_t n);
    int memcmp(const void *vl, const void *vr, size_t n);
    long unsigned strlen(const char s[]);
    int strncmp(const char *s1, const char *s2, unsigned long n);
    char *strcat(char *destination, const char *source);
    char *strcpy(char *destination, const char *source);
    char *strncpy(char *destination, const char *source, unsigned long num);
    int strcmp(const char *l, const char *r);
    char *strstr(const char *haystack, const char *needle);
    char *strdup(const char *String);
    char *strchr(const char *String, int Char);
    char *strrchr(const char *String, int Char);
    int strncasecmp(const char *lhs, const char *rhs, long unsigned int Count);
    int strcasecmp(const char *lhs, const char *rhs);
    char *strtok(char *__restrict__ src, const char *__restrict__ delim);

#ifdef __cplusplus
}
#endif
