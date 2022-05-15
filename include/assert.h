#pragma once

#ifdef syskernel
#include <types.h>
#include <sys.h>

START_EXTERNC

#define assert(x)     \
    do                \
    {                 \
        if (!(x))     \
            while (1) \
                ;     \
    } while (0)

#else

#define assert(x)     \
    do                \
    {                 \
        if (!(x))     \
            while (1) \
                ;     \
    } while (0)

#endif

#define ASSERT(x) assert(x)

// static_assert will throw a compile-time exception if assertion fails
#define static_assert(x) \
    switch (x)           \
    case 0:              \
    case (x):

#ifdef syskernel
END_EXTERNC
#endif
