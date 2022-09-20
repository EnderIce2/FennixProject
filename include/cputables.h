#pragma once

#ifndef __aarch64__

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
    uint32_t Reserved0;
    uint64_t StackPointer[3];
    uint64_t Reserved1;
    uint64_t InterruptStackTable[7];
    uint16_t Reserved2;
    uint16_t IOMapBaseAddressOffset;
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
    GlobalDescriptorTableEntry UserCode;
    GlobalDescriptorTableEntry UserData;
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
    uint64_t Present : 1;
    uint64_t BaseHigh : 48;
    uint64_t Reserved3 : 32;
} __attribute__((packed)) InterruptDescriptorTableEntry;

typedef struct _InterruptDescriptorTableDescriptor
{
    uint16_t Length;
    InterruptDescriptorTableEntry *Entries;
} __attribute__((packed)) InterruptDescriptorTableDescriptor;


#if defined(__amd64__)
typedef __UINT64_TYPE__ _cpu_uint_t;
typedef __UINT64_TYPE__ _cpu_raw_uint_t;
#elif defined(__i386__)
typedef __UINT32_TYPE__ _cpu_uint_t;
typedef __UINT64_TYPE__ _cpu_raw_uint_t;
#endif

typedef union
{
    struct
    {
        /** @brief Carry Flag */
        _cpu_uint_t CF : 1;
        /** @brief Reserved */
        _cpu_uint_t always_one : 1;
        /** @brief Parity Flag */
        _cpu_uint_t PF : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved0 : 1;
        /** @brief Auxiliary Carry Flag */
        _cpu_uint_t AF : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved1 : 1;
        /** @brief Zero Flag */
        _cpu_uint_t ZF : 1;
        /** @brief Sign Flag */
        _cpu_uint_t SF : 1;
        /** @brief Trap Flag */
        _cpu_uint_t TF : 1;
        /** @brief Interrupt Enable Flag */
        _cpu_uint_t IF : 1;
        /** @brief Direction Flag */
        _cpu_uint_t DF : 1;
        /** @brief Overflow Flag */
        _cpu_uint_t OF : 1;
        /** @brief I/O Privilege Level */
        _cpu_uint_t IOPL : 2;
        /** @brief Nested Task */
        _cpu_uint_t NT : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved2 : 1;
        /** @brief Resume Flag */
        _cpu_uint_t RF : 1;
        /** @brief Virtual 8086 Mode */
        _cpu_uint_t VM : 1;
        /** @brief Alignment Check */
        _cpu_uint_t AC : 1;
        /** @brief Virtual Interrupt Flag */
        _cpu_uint_t VIF : 1;
        /** @brief Virtual Interrupt Pending */
        _cpu_uint_t VIP : 1;
        /** @brief ID Flag */
        _cpu_uint_t ID : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved3 : 10;
    };
    _cpu_raw_uint_t raw;
} RFLAGS;

typedef union CR0
{
    struct
    {
        /** @brief Protection Enable */
        _cpu_uint_t PE : 1;
        /** @brief Monitor Coprocessor */
        _cpu_uint_t MP : 1;
        /** @brief Emulation */
        _cpu_uint_t EM : 1;
        /** @brief Task Switched */
        _cpu_uint_t TS : 1;
        /** @brief Extension Type */
        _cpu_uint_t ET : 1;
        /** @brief Numeric Error */
        _cpu_uint_t NE : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved0 : 10;
        /** @brief Write Protect */
        _cpu_uint_t WP : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved1 : 1;
        /** @brief Alignment Mask */
        _cpu_uint_t AM : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved2 : 10;
        /** @brief Mot Write-through */
        _cpu_uint_t NW : 1;
        /** @brief Cache Disable */
        _cpu_uint_t CD : 1;
        /** @brief Paging */
        _cpu_uint_t PG : 1;
    };
    _cpu_raw_uint_t raw;
} CR0;

typedef union CR2
{
    struct
    {
        /** @brief Page Fault Linear Address */
        _cpu_uint_t PFLA;
    };
    _cpu_raw_uint_t raw;
} CR2;

typedef union CR3
{
    struct
    {
        /** @brief Not used if bit 17 of CR4 is 1 */
        _cpu_uint_t PWT : 1;
        /** @brief Not used if bit 17 of CR4 is 1 */
        _cpu_uint_t PCD : 1;
        /** @brief Base of PML4T/PML5T */
        _cpu_uint_t PDBR;
    };
    _cpu_raw_uint_t raw;
} CR3;

typedef union CR4
{
    struct
    {
        /** @brief Virtual-8086 Mode Extensions */
        _cpu_uint_t VME : 1;
        /** @brief Protected-Mode Virtual Interrupts */
        _cpu_uint_t PVI : 1;
        /** @brief Time Stamp Disable */
        _cpu_uint_t TSD : 1;
        /** @brief Debugging Extensions */
        _cpu_uint_t DE : 1;
        /** @brief Page Size Extensions */
        _cpu_uint_t PSE : 1;
        /** @brief Physical Address Extension */
        _cpu_uint_t PAE : 1;
        /** @brief Machine Check Enable */
        _cpu_uint_t MCE : 1;
        /** @brief Page Global Enable */
        _cpu_uint_t PGE : 1;
        /** @brief Performance Monitoring Counter */
        _cpu_uint_t PCE : 1;
        /** @brief Operating System Support */
        _cpu_uint_t OSFXSR : 1;
        /** @brief Operating System Support */
        _cpu_uint_t OSXMMEXCPT : 1;
        /** @brief User-Mode Instruction Prevention */
        _cpu_uint_t UMIP : 1;
        /** @brief Linear Address 57bit */
        _cpu_uint_t LA57 : 1;
        /** @brief VMX Enable */
        _cpu_uint_t VMXE : 1;
        /** @brief SMX Enable */
        _cpu_uint_t SMXE : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved0 : 1;
        /** @brief FSGSBASE Enable */
        _cpu_uint_t FSGSBASE : 1;
        /** @brief PCID Enable */
        _cpu_uint_t PCIDE : 1;
        /** @brief XSAVE and Processor Extended States Enable */
        _cpu_uint_t OSXSAVE : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved1 : 1;
        /** @brief SMEP Enable */
        _cpu_uint_t SMEP : 1;
        /** @brief SMAP Enable */
        _cpu_uint_t SMAP : 1;
        /** @brief Protection-Key Enable */
        _cpu_uint_t PKE : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved2 : 9;
    };
    _cpu_raw_uint_t raw;
} CR4;

typedef union CR8
{
    struct
    {
        /** @brief Task Priority Level */
        _cpu_uint_t TPL : 1;
    };
    _cpu_raw_uint_t raw;
} CR8;

typedef union PageFaultErrorCode
{
    struct
    {
        /** @brief When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page. */
        _cpu_uint_t P : 1;
        /** @brief When set, the page fault was caused by a write access. When not set, it was caused by a read access. */
        _cpu_uint_t W : 1;
        /** @brief When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation. */
        _cpu_uint_t U : 1;
        /** @brief When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1. */
        _cpu_uint_t R : 1;
        /** @brief When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled. */
        _cpu_uint_t I : 1;
        /** @brief When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights. */
        _cpu_uint_t PK : 1;
        /** @brief When set, the page fault was caused by a shadow stack access. */
        _cpu_uint_t SS : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved0 : 8;
        /** @brief When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging. */
        _cpu_uint_t SGX : 1;
        /** @brief Reserved */
        _cpu_uint_t _reserved1 : 16;
    };
    _cpu_raw_uint_t raw;
} PageFaultErrorCode;

// ! TODO: UNTESTED!
typedef union SelectorErrorCode
{
    struct
    {
        /** @brief When set, the exception originated externally to the processor. */
        _cpu_uint_t External : 1;
        /** @brief IDT/GDT/LDT Table
         *  @details 0b00 - The Selector Index references a descriptor in the GDT.
         *  @details 0b01 - The Selector Index references a descriptor in the IDT.
         *  @details 0b10 - The Selector Index references a descriptor in the LDT.
         *  @details 0b11 - The Selector Index references a descriptor in the IDT.
         */
        _cpu_uint_t Table : 2;
        /** @brief The index in the GDT, IDT or LDT. */
        _cpu_uint_t Idx : 13;
        /** @brief Reserved */
        _cpu_uint_t Reserved : 16;
    };
    _cpu_raw_uint_t raw;
} SelectorErrorCode;

// ! TODO: UNTESTED!
typedef union DR7
{
    struct
    {
        /** @brief Local DR0 Breakpoint (0) */
        _cpu_uint_t LocalDR0 : 1;
        /** @brief Global DR0 Breakpoint (1) */
        _cpu_uint_t GlobalDR0 : 1;
        /** @brief Local DR1 Breakpoint (2) */
        _cpu_uint_t LocalDR1 : 1;
        /** @brief Global DR1 Breakpoint (3) */
        _cpu_uint_t GlobalDR1 : 1;
        /** @brief Local DR2 Breakpoint (4) */
        _cpu_uint_t LocalDR2 : 1;
        /** @brief Global DR2 Breakpoint (5) */
        _cpu_uint_t GlobalDR2 : 1;
        /** @brief Local DR3 Breakpoint (6) */
        _cpu_uint_t LocalDR3 : 1;
        /** @brief Global DR3 Breakpoint (7) */
        _cpu_uint_t GlobalDR3 : 1;
        /** @brief Reserved [7 - (16-17)] */
        _cpu_uint_t Reserved : 9;
        /** @brief Conditions for DR0 (16-17) */
        _cpu_uint_t ConditionsDR0 : 1;
        /** @brief Size of DR0 Breakpoint (18-19) */
        _cpu_uint_t SizeDR0 : 1;
        /** @brief Conditions for DR1 (20-21) */
        _cpu_uint_t ConditionsDR1 : 1;
        /** @brief Size of DR1 Breakpoint (22-23) */
        _cpu_uint_t SizeDR1 : 1;
        /** @brief Conditions for DR2 (24-25) */
        _cpu_uint_t ConditionsDR2 : 1;
        /** @brief Size of DR2 Breakpoint (26-27) */
        _cpu_uint_t SizeDR2 : 1;
        /** @brief Conditions for DR3 (28-29) */
        _cpu_uint_t ConditionsDR3 : 1;
        /** @brief Size of DR3 Breakpoint (30-31) */
        _cpu_uint_t SizeDR3 : 1;
    };
    _cpu_raw_uint_t raw;
} DR7;

// ! TODO: UNTESTED!
typedef union EFER
{
    struct
    {
        /** @brief Enable syscall & sysret instructions in 64-bit mode. */
        _cpu_uint_t SCE : 1;
        /** @brief Reserved */
        _cpu_uint_t Reserved0 : 7;
        /** @brief Enable long mode. */
        _cpu_uint_t LME : 1;
        /** @brief Reserved */
        _cpu_uint_t Reserved1 : 1;
        /** @brief Indicates long. */
        _cpu_uint_t LMA : 1;
        /** @brief Enable No-Execute Bit */
        _cpu_uint_t NXE : 1;
        /** @brief Enable Secure Virtual Machine */
        _cpu_uint_t SVME : 1;
        /** @brief Enable Long Mode Segment Limit */
        _cpu_uint_t LMSLE : 1;
        /** @brief Enable Fast FXSAVE/FXRSTOR */
        _cpu_uint_t FFXSR : 1;
        /** @brief Enable Translation Cache Extension */
        _cpu_uint_t TCE : 1;
        /** @brief Reserved */
        _cpu_uint_t Reserved2 : 32;
    };
    _cpu_raw_uint_t raw;
} __attribute__((packed)) EFER;

#endif // !__aarch64__
