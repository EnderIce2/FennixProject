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

enum Architecture
{
    UnknownArchitecture,
    x86,
    x64,
    ARM,
    ARM64
};

enum Platform
{
    UnknownPlatform,
    Native,
    Linux,
    POSIX,
    Windows
};

struct GeneralProcessInfo
{
    uint64_t SpawnTick = 0, UsedTicks = 0, StartUsedTicks = 0, EndUsedTicks = 0, LastUsedTicks = 0;
    uint64_t Year, Month, Day, Hour, Minute, Second;
    uint64_t Usage[256];
    enum Architecture Architecture;
    enum Platform Platform;
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
    struct PCB *Parent;
    void *Stack;
    char FXRegion[512] __attribute__((aligned(16)));
    REGISTERS Registers;
    uint64_t fs, gs, cs, ss, ds, es;
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
    CR3 PageTable;
    GeneralProcessInfo Info;
    GeneralSecurityInfo Security;
    Vector<TCB *> Threads;
    Vector<struct PCB *> Children;
    uint32_t Checksum;
};
