#ifndef _FSL_ALLOCATE_H
#define _FSL_ALLOCATE_H

#include <cdefs.h>
#include <stddef.h>

E void *malloc(size_t);
E void *realloc(void *, size_t);
E void *calloc(size_t, size_t);
E void free(void *);

#endif
