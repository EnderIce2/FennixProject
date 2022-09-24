#include <err.h>

#include "cpu/smp.hpp"

long SetError(long ErrorCode)
{
    debug("Setting error code to %d", ErrorCode);
    return CurrentCPU->ErrorCode = ErrorCode;
}

long GetError()
{
    debug("Getting error code %d", CurrentCPU->ErrorCode);
    return CurrentCPU->ErrorCode;
}
