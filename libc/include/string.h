#pragma once
#include <stdint.h>
#include <stddef.h>
#include <cdefs.h>

E void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n);
E void *memset(void *dest, int data, size_t nbytes);
E void *memmove(void *dest, const void *src, size_t n);
E int memcmp(const void *vl, const void *vr, size_t n);

E int strncmp(const char *s1, const char *s2, long unsigned int n);
E long unsigned strlen(const char s[]);
E char *strcat(char *destination, const char *source);
E char *strcpy(char *destination, const char *source);
E char *strncpy(char *destination, const char *source, unsigned long num);
E int strcmp(const char *l, const char *r);
E char *strstr(const char *haystack, const char *needle);
E char *strdup(const char *String);
E char *strchr(const char *String, int Char);
E char *strrchr(const char *String, int Char);
E int strncasecmp(const char *lhs, const char *rhs, long unsigned int Count);
E int strcasecmp(const char *lhs, const char *rhs);

E int isdigit(int Char);
E int isspace(int Char);
