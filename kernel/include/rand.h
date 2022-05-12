#pragma once
#include <types.h>

EXTERNC uint16_t rand16();
EXTERNC uint32_t rand32();
EXTERNC uint64_t rand64();
void changeseed(uint64_t seed);
