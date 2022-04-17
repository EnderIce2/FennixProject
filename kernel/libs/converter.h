#pragma once

#include <types.h>

START_EXTERNC

    const char *u64ToString(uint64_t value);
    const char *i64ToString(int64_t value);
    const char *u64ToHexString(uint64_t value);
    const char *u32ToHexString(uint32_t value);
    const char *u16ToHexString(uint16_t value);
    const char *u8ToHexString(uint8_t value);
    // const char *ToString(double value, uint8_t decimalPlaces);
    // const char *ToString(double value);

END_EXTERNC
