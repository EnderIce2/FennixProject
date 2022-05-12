#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <easyc.h>

typedef char const *string;

#define asm __asm__

#define AND &&
#define AND_EQ &=
#define EQ ==
#define BITAND &
#define BITOR |
#define COMPL ~
#define NOT !
#define NOT_EQ !=
#define OR ||
#define OR_EQ |=
#define XOR ^
#define XOR_EQ ^=

#define UNUSED(x) (void)(x)

#define ALIGN_UP(x, align) ((__typeof__(x))(((uint64_t)(x) + ((align)-1)) & (~((align)-1))))
#define ALIGN_DOWN(x, align) ((__typeof__(x))((x) & (~((align)-1))))

#define badfennec 0xBADFE2EC // ASCII Binary Character Table. 110 is N. It has two one's
#define deadcode 0xDEADC0DE
#define deadbeef 0xDEADBEEF
#define deadcafe 0xDEADCAFE
#define deadbabe 0xDEADBABE
#define badbabe 0xBADCAFE
#define badcafe 0xBADBABE
#define defect 0xDEFEC7
#define dead 0xDEAD

#define CPU_STOP \
    for (;;)     \
        asm volatile("hlt")

#ifdef __cplusplus

#define EXTERNC extern "C"
#define START_EXTERNC \
    extern "C"        \
    {
#define END_EXTERNC \
    }

#else

#define EXTERNC
#define START_EXTERNC
#define END_EXTERNC

#endif

#ifndef __cplusplus

#ifdef __STDC__
#ifdef __STDC_VERSION__
#if (__STDC_VERSION__ >= 201710L)
#define C_LANGUAGE_STANDARD 2018
#elif (__STDC_VERSION__ >= 201112L)
#define C_LANGUAGE_STANDARD 2011
#elif (__STDC_VERSION__ >= 199901L)
#define C_LANGUAGE_STANDARD 1999
#elif (__STDC_VERSION__ >= 199409L)
#define C_LANGUAGE_STANDARD 1995
#endif
#else
#define C_LANGUAGE_STANDARD 1990
#endif
#else
#define C_LANGUAGE_STANDARD 1972
#endif

#else

#ifdef __STDC__
#ifdef __cplusplus
#if (__cplusplus >= 202100L)
#define CPP_LANGUAGE_STANDARD 2023
#elif (__cplusplus >= 202002L)
#define CPP_LANGUAGE_STANDARD 2020
#elif (__cplusplus >= 201703L)
#define CPP_LANGUAGE_STANDARD 2017
#elif (__cplusplus >= 201402L)
#define CPP_LANGUAGE_STANDARD 2014
#elif (__cplusplus >= 201103L)
#define CPP_LANGUAGE_STANDARD 2011
#elif (__cplusplus >= 199711L)
#define CPP_LANGUAGE_STANDARD 1998
#endif
#else
#define CPP_LANGUAGE_STANDARD __cplusplus
#endif
#else
#define CPP_LANGUAGE_STANDARD __cplusplus
#endif

#endif

#define APICALL __attribute__((__cdecl__))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define VPOKE(type, address) (*((volatile type *)(address)))
#define POKE(type, address) (*((type *)(address)))

// kilobyte
#define TO_KB(d) (d / 1024)
// megabyte
#define TO_MB(d) (d / 1024 / 1024)
// gigabyte
#define TO_GB(d) (d / 1024 / 1024 / 1024)
// terabyte
#define TO_TB(d) (d / 1024 / 1024 / 1024 / 1024)
// petabyte
#define TO_PB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024)
// exobyte
#define TO_EB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024 / 1024)
// zettabyte
#define TO_ZB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024)
// yottabyte
#define TO_YB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024)
// brontobyte
#define TO_BB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024)
// geopbyte
#define TO_GPB(d) (d / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024 / 1024)
// there's more but why i should add more??
