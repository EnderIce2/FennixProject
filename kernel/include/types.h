#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>

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
        asm volatile("hlt");

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

#define FENAPI
#define APICALL __attribute__((__cdecl__))

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