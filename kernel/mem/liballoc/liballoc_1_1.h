#ifndef _LIBALLOC_H
#define _LIBALLOC_H

#define libPREFIX(func) libk##func

#include <types.h>

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
