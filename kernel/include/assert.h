#pragma once

#include <types.h>
#include <sys.h>

START_EXTERNC

#define assert(x)                       \
    do                                  \
    {                                   \
        if (!(x))                       \
            panic("Assertion failed!"); \
    } while (0)

#define ASSERT(x) assert(x)

// static_assert will throw a compile-time exception if assertion fails
#define static_assert(x) \
    switch (x)           \
    case 0:              \
    case (x):

END_EXTERNC