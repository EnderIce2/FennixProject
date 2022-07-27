#ifndef _FSL_PRINT_H
#define _FSL_PRINT_H

#include <file.h>
#include <stdarg.h>

E FILE *fslin;
E FILE *fslout;
E FILE *fslerr;

E int fprintf(FILE *Stream, const char *Format, ...);
E int sprintf(char *s, const char *format, ...);
E int printf(const char *Format, ...);
E int vfprintf(FILE *Stream, const char *Format, va_list Args);
E int vprintf(const char *Format, va_list Args);
E int snprintf(char *String, size_t Length, const char *Format, ...);
E int vsnprintf(char *String, size_t Length, const char *Format, va_list Args);
E int sscanf(const char *String, const char *Format, ...);
E int fputs(const char *String, FILE *Stream);
E int puts(const char *String);
E int fputc(int Char, FILE *Stream);
E int putchar(int Char);
E int fflush(FILE *Stream);

#endif
