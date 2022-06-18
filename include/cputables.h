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
    uint32_t Reserved0;
    uint64_t StackPointer[3];
    uint64_t Reserved1;
    uint64_t InterruptStackTable[7];
    uint64_t Reserved2;
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
    uint64_t Present : 1;
    uint64_t BaseHigh : 48;
    uint64_t Reserved3 : 32;
} __attribute__((packed)) InterruptDescriptorTableEntry;

typedef struct _InterruptDescriptorTableDescriptor
{
    uint16_t Length;
    InterruptDescriptorTableEntry *Entries;
} __attribute__((packed)) InterruptDescriptorTableDescriptor;

typedef union
{
    struct
    {
        /** @brief Carry Flag */
        uint64_t CF : 1;
        /** @brief Reserved */
        uint64_t always_one : 1;
        /** @brief Parity Flag */
        uint64_t PF : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 1;
        /** @brief Auxiliary Carry Flag */
        uint64_t AF : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief Zero Flag */
        uint64_t ZF : 1;
        /** @brief Sign Flag */
        uint64_t SF : 1;
        /** @brief Trap Flag */
        uint64_t TF : 1;
        /** @brief Interrupt Enable Flag */
        uint64_t IF : 1;
        /** @brief Direction Flag */
        uint64_t DF : 1;
        /** @brief Overflow Flag */
        uint64_t OF : 1;
        /** @brief I/O Privilege Level */
        uint64_t IOPL : 2;
        /** @brief Nested Task */
        uint64_t NT : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 1;
        /** @brief Resume Flag */
        uint64_t RF : 1;
        /** @brief Virtual 8086 Mode */
        uint64_t VM : 1;
        /** @brief Alignment Check */
        uint64_t AC : 1;
        /** @brief Virtual Interrupt Flag */
        uint64_t VIF : 1;
        /** @brief Virtual Interrupt Pending */
        uint64_t VIP : 1;
        /** @brief ID Flag */
        uint64_t ID : 1;
        /** @brief Reserved */
        uint64_t _reserved3 : 10;
    };
    uint64_t raw;
} RFLAGS;

typedef union CR0
{
    struct
    {
        /** @brief Protection Enable */
        uint64_t PE : 1;
        /** @brief Monitor Coprocessor */
        uint64_t MP : 1;
        /** @brief Emulation */
        uint64_t EM : 1;
        /** @brief Task Switched */
        uint64_t TS : 1;
        /** @brief Extension Type */
        uint64_t ET : 1;
        /** @brief Numeric Error */
        uint64_t NE : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 10;
        /** @brief Write Protect */
        uint64_t WP : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief Alignment Mask */
        uint64_t AM : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 10;
        /** @brief Mot Write-through */
        uint64_t NW : 1;
        /** @brief Cache Disable */
        uint64_t CD : 1;
        /** @brief Paging */
        uint64_t PG : 1;
    };
    uint64_t raw;
} CR0;

typedef union CR2
{
    struct
    {
        /** @brief Page Fault Linear Address */
        uint64_t PFLA;
    };
    uint64_t raw;
} CR2;

typedef union CR3
{
    struct
    {
        /** @brief Not used if bit 17 of CR4 is 1 */
        uint64_t PWT : 1;
        /** @brief Not used if bit 17 of CR4 is 1 */
        uint64_t PCD : 1;
        /** @brief Base of PML4T/PML5T */
        uint64_t PDBR;
    };
    uint64_t raw;
} CR3;

typedef union CR4
{
    struct
    {
        /** @brief Virtual-8086 Mode Extensions */
        uint64_t VME : 1;
        /** @brief Protected-Mode Virtual Interrupts */
        uint64_t PVI : 1;
        /** @brief Time Stamp Disable */
        uint64_t TSD : 1;
        /** @brief Debugging Extensions */
        uint64_t DE : 1;
        /** @brief Page Size Extensions */
        uint64_t PSE : 1;
        /** @brief Physical Address Extension */
        uint64_t PAE : 1;
        /** @brief Machine Check Enable */
        uint64_t MCE : 1;
        /** @brief Page Global Enable */
        uint64_t PGE : 1;
        /** @brief Performance Monitoring Counter */
        uint64_t PCE : 1;
        /** @brief Operating System Support */
        uint64_t OSFXSR : 1;
        /** @brief Operating System Support */
        uint64_t OSXMMEXCPT : 1;
        /** @brief User-Mode Instruction Prevention */
        uint64_t UMIP : 1;
        /** @brief Linear Address 57bit */
        uint64_t LA57 : 1;
        /** @brief VMX Enable */
        uint64_t VMXE : 1;
        /** @brief SMX Enable */
        uint64_t SMXE : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 1;
        /** @brief FSGSBASE Enable */
        uint64_t FSGSBASE : 1;
        /** @brief PCID Enable */
        uint64_t PCIDE : 1;
        /** @brief XSAVE and Processor Extended States Enable */
        uint64_t OSXSAVE : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief SMEP Enable */
        uint64_t SMEP : 1;
        /** @brief SMAP Enable */
        uint64_t SMAP : 1;
        /** @brief Protection-Key Enable */
        uint64_t PKE : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 9;
    };
    uint64_t raw;
} CR4;

typedef union CR8
{
    struct
    {
        /** @brief Task Priority Level */
        uint64_t TPL : 1;
    };
    uint64_t raw;
} CR8;

typedef union PageFaultErrorCode
{
    struct
    {
        /** @brief When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page. */
        uint64_t P : 1;
        /** @brief 	When set, the page fault was caused by a write access. When not set, it was caused by a read access. */
        uint64_t W : 1;
        /** @brief When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation. */
        uint64_t U : 1;
        /** @brief When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1. */
        uint64_t R : 1;
        /** @brief When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled. */
        uint64_t I : 1;
        /** @brief When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights. */
        uint64_t PK : 1;
        /** @brief When set, the page fault was caused by a shadow stack access. */
        uint64_t SS : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 8;
        /** @brief When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging. */
        uint64_t SGX : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 16;
    };
    uint64_t raw;
} PageFaultErrorCode;

// ! TODO: UNTESTED!
typedef union SelectorErrorCode
{
    struct
    {
        /** @brief When set, the exception originated externally to the processor. */
        uint64_t External : 1;
        /** @brief IDT/GDT/LDT Table
         *  @details 0b00 - The Selector Index references a descriptor in the GDT.
         *  @details 0b01 - The Selector Index references a descriptor in the IDT.
         *  @details 0b10 - The Selector Index references a descriptor in the LDT.
         *  @details 0b11 - The Selector Index references a descriptor in the IDT.
         */
        uint64_t Table : 2;
        /** @brief The index in the GDT, IDT or LDT. */
        uint64_t Idx : 13;
        /** @brief Reserved */
        uint64_t Reserved : 16;
    };
    uint64_t raw;
} SelectorErrorCode;
