#pragma once

#include <types.h>
#include <cpuid.h>

/* DEPRECATED: You should use the <cpuid.h> header that comes with GCC instead. */

enum cpuid_requests
{
    CPUID_GETVENDORSTRING,
    CPUID_GETFEATURES,
    CPUID_GETTLB,
    CPUID_GETSERIAL,

    CPUID_INTELEXTENDED = 0x80000000,
    CPUID_INTELFEATURES,
    CPUID_INTELBRANDSTRING,
    CPUID_INTELBRANDSTRINGMORE,
    CPUID_INTELBRANDSTRINGEND,
};

// https://wiki.osdev.org/CPUID

static inline int cpuid_string(int code, int where[4])
{
#if defined(__amd64__) || defined(__i386__)
    asm volatile("cpuid"
                 : "=a"(*where), "=b"(*(where + 0)),
                   "=d"(*(where + 1)), "=c"(*(where + 2))
                 : "a"(code));
#endif
    return (int)where[0];
}

/**
 * @brief Check if the specified CPU feature is supported.
 *
 * @param feature The CPU feature to check.
 * @return bool
 */
EXTERNC bool cpu_feature(enum CPU_FEATURE feature);
EXTERNC char *cpu_get_info();
