#ifndef _LIBALLOC_H
#define _LIBALLOC_H

// #define libPREFIX(func) ##func
#define libPREFIX(func) func

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    extern void *libPREFIX(malloc)(size_t);          ///< The standard function.
    extern void *libPREFIX(realloc)(void *, size_t); ///< The standard function.
    extern void *libPREFIX(calloc)(size_t, size_t);  ///< The standard function.
    extern void libPREFIX(free)(void *);             ///< The standard function.

#ifdef __cplusplus
}
#endif

#endif
