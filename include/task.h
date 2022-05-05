#pragma once
#include <stdint.h>
#include <interrupts.h>
#ifdef __cplusplus
#include <vector.hpp>
#endif

enum ControlBlockState
{
    STATE_UNKNOWN,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED,
};

enum ControlBlockPolicy
{
    POLICY_UNKNOWN,
    POLICY_KERNEL,
    POLICY_SYSTEM,
    POLICY_USER,
};

enum ControlBlockPriority
{
    PRIORITY_UNKNOWN,
    PRIORITY_REALTIME,
    PRIORITY_VERYHIGH,
    PRIORITY_HIGH,
    PRIORITY_MEDIUM,
    PRIORITY_LOW,
    PRIORITY_VERYLOW,
    /* ... */
    PRIORITY_IDLE = 500,
};

typedef struct _ControlBlockTime
{
    uint64_t ticks_used;
    uint64_t tick;
    uint64_t y;
    uint64_t M;
    uint64_t d;
    uint64_t h;
    uint64_t m;
    uint64_t s;
} ControlBlockTime;

typedef struct _Segments
{
    uint64_t fs;
    uint64_t gs;
    uint64_t cs;
    uint64_t ss;
    uint64_t ds;
    uint64_t es;
} Segments;

typedef struct _ThreadControlBlock
{
    uint64_t ThreadID;
    enum ControlBlockState State;
    enum ControlBlockPolicy Policy;
    enum ControlBlockPriority Priority;
    struct _ProcessControlBlock *Parent;
    struct MessageQueue *Msg;
    void *Stack;
    bool UserMode;
    REGISTERS Registers;
    char fx_region[512] __attribute__((aligned(16)));
    Segments Segment;
    uint64_t ExitCode;
    ControlBlockTime *Time;
    uint32_t Checksum;
} ThreadControlBlock;

typedef struct _ProcessControlBlock
{
    uint64_t ProcessID;
    char Name[256];
    enum ControlBlockState State;
    struct _ProcessControlBlock *Parent;
#ifdef __cplusplus
    Vector<struct _ProcessControlBlock *> Children;
#else
    void *Childern;
#endif
    uint64_t ExitCode;
    void *PageTable;
    ControlBlockTime *Time;
#ifdef __cplusplus
    Vector<ThreadControlBlock *> Threads;
#else
    void *Threads; // not supported in C
#endif
    uint32_t Checksum;
} ProcessControlBlock;