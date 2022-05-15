#pragma once
#include <stdint.h>
#include <interrupts.h>
#include <vector.hpp>

enum Checksum
{
    PROCESS_CHECKSUM = 0xCAFEBABE,
    THREAD_CHECKSUM = 0xDEADCAFE
};

enum ELEVATION
{
    UnknownElevation,
    Kernel,
    System,
    Idle,
    User
};

enum STATUS
{
    UnknownStatus,
    Ready,
    Running,
    Waiting,
    Terminated
};

struct GeneralProcessInfo
{
    uint64_t SpawnTick = 0, UsedTicks = 0, StartUsedTicks = 0, EndUsedTicks = 0, LastUsedTicks = 0;
    uint64_t Usage[256];
    uint64_t Year, Month, Day, Hour, Minute, Second;
    int Priority;
};

struct GeneralSecurityInfo
{
    uint64_t Token;
};

struct PCB;

struct TCB
{
    uint64_t ID;
    char Name[256];
    STATUS Status;
    uint64_t ExitCode;
    PCB *Parent;
    void *Stack;
    REGISTERS Registers;
    uint64_t fs, gs, cs, ss, ds, es;
    char FXRegion[512] __attribute__((aligned(16)));
    GeneralProcessInfo Info;
    GeneralSecurityInfo Security;
    uint32_t Checksum;
};

struct PCB
{
    uint64_t ID;
    char Name[256];
    STATUS Status;
    ELEVATION Elevation;
    uint64_t ExitCode;
    struct PCB *Parent;
    Vector<struct PCB *> Children;
    CR3 PageTable;
    Vector<TCB *> Threads;
    GeneralProcessInfo Info;
    GeneralSecurityInfo Security;
    uint32_t Checksum;
};
