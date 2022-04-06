#pragma once
#include <stdint.h>

typedef struct _TaskStateSegmentEntry
{
    uint16_t Length;
    uint16_t Low;
    uint8_t Middle;
    uint8_t Flags1;
    uint8_t Flags2;
    uint8_t High;
    uint32_t Upper32;
    uint32_t Reserved;
} __attribute__((packed)) TaskStateSegmentEntry;

typedef struct _TaskStateSegment
{
    uint32_t Reserved1;
    uint64_t StackPointer0;
    uint64_t StackPointer1;
    uint64_t StackPointer2;
    uint64_t Reserved2;
    uint64_t InterruptStackTable1;
    uint64_t InterruptStackTable2;
    uint64_t InterruptStackTable3;
    uint64_t InterruptStackTable4;
    uint64_t InterruptStackTable5;
    uint64_t InterruptStackTable6;
    uint64_t InterruptStackTable7;
    uint64_t Reserved3;
    uint32_t IOMapBaseAddressOffset;
} __attribute__((packed)) TaskStateSegment;

typedef struct _GlobalDescriptorTableEntry
{
    /** @brief Length [Bits 0-15] */
    uint16_t Length;
    /** @brief Low Base [Bits 0-15] */
    uint16_t BaseLow;
    /** @brief Middle Base [Bits 0-23] */
    uint8_t BaseMiddle;
    /** @brief Access */
    uint8_t Access;
    /** @brief Flags [Bits 16-19] */
    uint8_t Flags;
    /** @brief High Base [Bits 24-31] */
    uint8_t BaseHigh;
} __attribute__((packed)) GlobalDescriptorTableEntry;

typedef struct _GlobalDescriptorTableEntries
{
    GlobalDescriptorTableEntry Null;
    GlobalDescriptorTableEntry Code;
    GlobalDescriptorTableEntry Data;
    GlobalDescriptorTableEntry UserData;
    GlobalDescriptorTableEntry UserCode;
    TaskStateSegmentEntry TaskStateSegment;
} __attribute__((packed)) GlobalDescriptorTableEntries;

typedef struct _GlobalDescriptorTableDescriptor
{
    /** @brief GDT entries length */
    uint16_t Length;
    /** @brief GDT entries address */
    GlobalDescriptorTableEntries *Entries;
} __attribute__((packed)) GlobalDescriptorTableDescriptor;

typedef enum _InterruptDescriptorTableFlags
{
    FlagGate_TASK = 0b0101,
    FlagGate16BIT_INT = 0b0110,
    FlagGate16BIT_TRAP = 0b0111,
    FlagGate32BIT_INT = 0b1110,
    FlagGate32BIT_TRAP = 0b1111,
} InterruptDescriptorTableFlags;

typedef struct _InterruptDescriptorTableEntry
{
    uint64_t BaseLow : 16;
    uint64_t SegmentSelector : 16;
    uint64_t InterruptStackTable : 3;
    uint64_t Reserved1 : 5;
    InterruptDescriptorTableFlags Flags : 4;
    uint64_t Reserved2 : 1;
    uint64_t Ring : 2;
    uint64_t P : 1;
    uint64_t BaseHigh : 48;
    uint64_t Reserved3 : 32;
} __attribute__((packed)) InterruptDescriptorTableEntry;

typedef struct _InterruptDescriptorTableDescriptor
{
    uint16_t Length;
    InterruptDescriptorTableEntry *Entries;
} __attribute__((packed)) InterruptDescriptorTableDescriptor;

typedef union CR0
{
    struct
    {
        uint32_t PE : 1;          // Protection Enable
        uint32_t MP : 1;          // Monitor Coprocessor
        uint32_t EM : 1;          // Emulation
        uint32_t TS : 1;          // Task Switched
        uint32_t ET : 1;          // Extension Type
        uint32_t NE : 1;          // Numeric Error
        uint32_t _reserved0 : 10; // Reserved
        uint32_t WP : 1;          // Write Protect
        uint32_t _reserved1 : 1;  // Reserved
        uint32_t AM : 1;          // Alignment Mask
        uint32_t _reserved2 : 10; // Reserved
        uint32_t NW : 1;          // Mot Write-through
        uint32_t CD : 1;          // Cache Disable
        uint32_t PG : 1;          // Paging
    };
    uint64_t raw;
} CR0;

typedef union CR2
{
    struct
    {
        uint32_t PFLA; // Page Fault Linear Address
    };
    uint64_t raw;
} CR2;

typedef union CR3
{
    struct
    {
        uint32_t PWT : 1; // (Not used if bit 17 of CR4 is 1)
        uint32_t PCD : 1; // (Not used if bit 17 of CR4 is 1)
        uint32_t PDBR;    // Base of PML4T/PML5T
    };
    uint64_t raw;
} CR3;

typedef union CR4
{
    struct
    {
        uint32_t VME : 1;        // Virtual-8086 Mode Extensions
        uint32_t PVI : 1;        // Protected-Mode Virtual Interrupts
        uint32_t TSD : 1;        // Time Stamp Disable
        uint32_t DE : 1;         // Debugging Extensions
        uint32_t PSE : 1;        // Page Size Extensions
        uint32_t PAE : 1;        // Physical Address Extension
        uint32_t MCE : 1;        // Machine Check Enable
        uint32_t PGE : 1;        // Page Global Enable
        uint32_t PCE : 1;        // Performance Monitoring Counter
        uint32_t OSFXSR : 1;     // Operating System Support
        uint32_t OSXMMEXCPT : 1; // Operating System Support
        uint32_t UMIP : 1;       // User-Mode Instruction Prevention
        uint32_t LA57 : 1;       // Linear Address 57bit
        uint32_t VMXE : 1;       // VMX Enable
        uint32_t SMXE : 1;       // SMX Enable
        uint32_t _reserved3 : 1; // Reserved
        uint32_t FSGSBASE : 1;   // FSGSBASE Enable
        uint32_t PCIDE : 1;      // PCID Enable
        uint32_t OSXSAVE : 1;    // XSAVE and Processor Extended States Enable
        uint32_t _reserved4 : 1; // Reserved
        uint32_t SMEP : 1;       // SMEP Enable
        uint32_t SMAP : 1;       // SMAP Enable
        uint32_t PKE : 1;        // Protection-Key Enable
        uint32_t _reserved5 : 9; // Reserved
    };
    uint64_t raw;
} CR4;

typedef union CR8
{
    struct
    {
        uint32_t TPL : 1; // Task Priority Level
    };
    uint64_t raw;
} CR8;