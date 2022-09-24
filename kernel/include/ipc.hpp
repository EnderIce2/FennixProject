#pragma once
#include <events.hpp>
#include <lock.h>

namespace InterProcessCommunication
{
    typedef unsigned int IPCPort;

    enum IPCOperationType
    {
        IPCOperationNone,
        IPCOperationWrite,
        IPCOperationRead
    };

    enum IPCErrorCode
    {
        IPCUnknown,
        IPCSuccess,
        IPCNotListening,
        IPCTimeout,
        IPCInvalidPort,
        IPCPortInUse,
        IPCPortNotRegistered
    };

    typedef struct
    {
        int PID;
        int Length;
        void *Buffer;
        bool Listening;
        IPCOperationType Type;
        IPCErrorCode Error;
        LOCK Lock;
    } IPCHandle;

    typedef struct
    {
        int PID;
        int Length;
        IPCOperationType Type;
        IPCErrorCode Error;
        void *Buffer;

        // Reserved
        IPCHandle *HandleBuffer;
    } __attribute__((packed)) IPCSyscallHandle;

    struct IPCError
    {
        uint64_t ErrorCode;
    };
}
