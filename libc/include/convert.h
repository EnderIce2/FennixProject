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

/**
 * @brief Exchange values of two objects
 *
 * @param x
 * @param y
 * @return void
 */
E void swap(char *x, char *y);

/**
 * @brief
 *
 * @param Buffer
 * @param i
 * @param j
 * @return char*
 */
E char *reverse(char *Buffer, int i, int j);

/**
 * @brief Convert string to integer.
 *
 * @param String String to convert to integer.
 * @return Converted integral number as an int value.
 */
E int atoi(const char *String);

/**
 * @brief Convert string to double.
 * 
 * @param String String to convert to double.
 * @return Converted floating point as a double value.
 */
E double atof(const char *String);

/**
 * @brief Convert integer to string.
 * 
 * @param Value Value to be converted to a string.
 * @param Buffer Array in memory where to store the resulting null-terminated string.
 * @param Base Numerical base used to represent the value as a string, between 2 and 36, where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.
 * @return A pointer to the resulting null-terminated string.
 */
E char *itoa(int Value, char *Buffer, int Base);

#endif
