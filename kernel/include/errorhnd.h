#pragma once

enum ErrorCodes
{
    ERR_SUCCESS = 0,
    ERR_INVALID_ARGUMENT,
    ERR_INVALID_HANDLE,
    ERR_INVALID_OPERATION,
    ERR_INVALID_ADDRESS,
    ERR_INVALID_MEMORY,
    ERR_INVALID_MEMORY_SIZE,
    ERR_INVALID_MEMORY_ACCESS,
    ERR_INVALID_MEMORY_OPERATION
};

long SetError(long ErrorCode);
long GetError();
