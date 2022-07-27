#ifndef _FSL_CONVERT_H
#define _FSL_CONVERT_H

#include <cdefs.h>

/**
 * @brief Returns the absolute value of an integer.
 * 
 * @param i Integral value.
 * @return Absolute value of i.
 */
E int abs(int i);
E void swap(char *x, char *y);
E char *reverse(char *Buffer, int i, int j);
E int atoi(const char *String);
E double atof(const char *String);
E char *itoa(int Value, char *Buffer, int Base);

#endif
