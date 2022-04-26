#pragma once

#undef NULL
#ifdef __GNUG__
#define NULL __null
#else
#ifndef __cplusplus
#define NULL ((void *)0)
#else
#define NULL 0
#endif
#endif

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ long unsigned int
#endif
typedef __SIZE_TYPE__ size_t;

typedef long ssize_t;

#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ long int
#endif
typedef __PTRDIFF_TYPE__ ptrdiff_t;

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || (defined(__cplusplus) && __cplusplus >= 201103L)
typedef struct
{
    long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
    long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
#ifdef __i386__
    __float128 __max_align_f128 __attribute__((__aligned__(__alignof(__float128))));
#endif
} max_align_t;

#endif

#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
