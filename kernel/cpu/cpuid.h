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

/**
 * @brief Char array that should be 12 or 13 characters
 *
 */
typedef char *CPU_VENDOR;

typedef struct _CPU_INFO
{
    CPU_VENDOR vendor;
    char *name;
    int architecture;
    bool temperature_sensor;
    enum CPU_FEATURE feature;
} CPU_INFO;

static inline int cpuid_string(int code, int where[4])
{
    asm volatile("cpuid"
                 : "=a"(*where), "=b"(*(where + 0)),
                   "=d"(*(where + 1)), "=c"(*(where + 2))
                 : "a"(code));
    return (int)where[0];
}

CPU_VENDOR cpu_vendor();
/**
 * @brief Check if the specified CPU feature is supported.
 *
 * @param feature The CPU feature to check.
 * @return bool
 */
EXTERNC bool cpu_feature(enum CPU_FEATURE feature);
EXTERNC char *cpu_get_info();
