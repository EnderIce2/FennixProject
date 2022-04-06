#include <rand.h>
#include <asm.h>
#include "cpuid.h"

extern int asmRand16(uint16_t *buffer);
extern int asmRand32(uint32_t *buffer);
extern int asmRand64(uint64_t *buffer);

uint16_t *rand16()
{
    if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    {
        uint16_t *buf;
        unsigned int retry = 10;
        do
        {
            if (asmRand16(buf))
                return buf;
        } while (--retry);
        err("RDRAND intruction failed or not supported");
        return 0;
    }
    fixme("return something even if the rdrand is not supported");
    return 0;
}

uint32_t *rand32()
{
    if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    {
        uint32_t *buf;
        unsigned int retry = 10;
        do
        {
            if (asmRand32(buf))
                return buf;
        } while (--retry);
        err("RDRAND intruction failed or not supported");
        return 0;
    }
    fixme("return something even if the rdrand is not supported");
    return 0;
}

uint64_t *rand64()
{
    if (cpu_feature(CPUID_FEAT_RCX_RDRAND))
    {
        uint64_t *buf;
        unsigned int retry = 10;
        do
        {
            if (asmRand64(buf))
                return buf;
        } while (--retry);
        err("RDRAND intruction failed or not supported");
        return 0;
    }
    fixme("return something even if the rdrand is not supported");
    return 0;
}
