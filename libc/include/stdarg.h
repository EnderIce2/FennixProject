#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC_VA_LIST
#define __GNUC_VA_LIST
    typedef __builtin_va_list __gnuc_va_list;
#endif
#ifndef __va_list__
typedef __builtin_va_list va_list;
#endif

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)

#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L || __cplusplus + 0 >= 201103L
#define va_copy(d, s) __builtin_va_copy(d, s)
#endif

#ifdef __cplusplus
}
#endif