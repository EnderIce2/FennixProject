#include <rand.h>
#include <asm.h>
#include "cpuid.h"

// TODO: True RDRAND usage

static uint64_t randseed = deadbeef;

uint16_t rand16()
{
    randseed = randseed * 1103515245 + 12345;
    return (uint16_t)(randseed / 65536) % __UINT16_MAX__;

    // if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    // {
    //     uint16_t *buf;
    //     unsigned int retry = 10;
    //     do
    //     {
    //         if (asmRand16(buf))
    //             return buf;
    //     } while (--retry);
    //     err("RDRAND intruction failed or not supported");
    //     return 0;
    // }
    // fixme("return something even if the rdrand is not supported");
    // return 0;
}

uint32_t rand32()
{
    randseed = randseed * 1103515245 + 12345;
    return (uint32_t)(randseed / 65536) % __UINT32_MAX__;

    // if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    // {
    //     uint32_t *buf;
    //     unsigned int retry = 10;
    //     do
    //     {
    //         if (asmRand32(buf))
    //             return buf;
    //     } while (--retry);
    //     err("RDRAND intruction failed or not supported");
    //     return 0;
    // }
    // fixme("return something even if the rdrand is not supported");
    // return 0;
}

uint64_t rand64()
{
    randseed = randseed * 1103515245 + 12345;
    return (uint64_t)(randseed / 65536) % __UINT64_MAX__;

    // if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    // {
    //     uint64_t *buf;
    //     unsigned int retry = 10;
    //     do
    //     {
    //         if (asmRand64(buf))
    //             return buf;
    //     } while (--retry);
    //     err("RDRAND intruction failed or not supported");
    //     return 0;
    // }
    // fixme("return something even if the rdrand is not supported");
    // return 0;
}

void changeseed(uint64_t seed) { randseed = seed; }
