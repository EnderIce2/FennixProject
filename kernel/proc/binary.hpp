#pragma once

#include <stdint.h>
#include <task.h>

enum BinType
{
    BIN_TYPE_UNKNOWN,
    BIN_TYPE_ELF,
    BIN_TYPE_PE,
    BIN_TYPE_NE,
    BIN_TYPE_MZ,
};

BinType GetBinaryType(const char *Path);
PCB *ExecuteBinary(const char *Path, uint64_t Arg0, uint64_t Arg1, ELEVATION Elevation);
