#pragma once

#define assert(x)     \
    do                \
    {                 \
        if (!(x))     \
            while (1) \
                ;     \
    } while (0)

#define ASSERT(x) assert(x)

// static_assert will throw a compile-time exception if assertion fails
#define static_assert(x) \
    switch (x)           \
    case 0:              \
    case (x):
