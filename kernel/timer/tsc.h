#pragma once
#include <types.h>

EXTERNC void TSC_sleep(uint64_t Nanoseconds);
EXTERNC void TSC_oneshot(uint32_t Vector, uint64_t Miliseconds);
