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

enum TokenTrustLevel
{
    UnknownTrustLevel, // error
    Untrusted, // process is not trusted by the kernel
    Trusted, // trusted but have limits
    TrustedByKernel // can run all syscalls
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
    uint64_t SpawnTime = 0, UsedTime = 0, OldUsedTime = 0;
    uint64_t OldSystemTime = 0, CurrentSystemTime = 0;
    uint64_t Year, Month, Day, Hour, Minute, Second;
    uint64_t Usage[256];
    enum Architecture Architecture;
    enum Platform Platform;
    int Priority;
    bool Affinity[256];
};

struct GeneralSecurityInfo
{
    uint64_t Token;
};

typedef struct _ThreadRegisters
{
    uint64_t r15;        // General purpose
    uint64_t r14;        // General purpose
    uint64_t r13;        // General purpose
    uint64_t r12;        // General purpose
    uint64_t r11;        // General purpose
    uint64_t r10;        // General purpose
    uint64_t r9;         // General purpose
    uint64_t r8;         // General purpose
    uint64_t rbp;        // Base Pointer (meant for stack frames)
    uint64_t rdi;        // First Argument
    uint64_t rsi;        // Second Argument
    uint64_t rdx;        // Data (commonly extends the A register)
    uint64_t rcx;        // Counter
    uint64_t rbx;        // Base
    uint64_t rax;        // Accumulator
    uint64_t int_num;    // Interrupt Number
    uint64_t error_code; // Error code
    uint64_t rip;        // Instruction Pointer
    uint64_t cs;         // Code Segment
    RFLAGS rflags;       // Register Flags
    uint64_t rsp;        // Stack Pointer
    uint64_t ss;         // Stack Segment / Data Segment
} ThreadRegisters;

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
    ThreadRegisters Registers;
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
    uint64_t Offset;
    struct PCB *Parent;
    CR3 PageTable;
    GeneralProcessInfo Info;
    GeneralSecurityInfo Security;
    Vector<TCB *> Threads;
    Vector<struct PCB *> Children;
    uint32_t Checksum;
};
