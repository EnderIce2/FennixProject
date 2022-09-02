#pragma once

#include <stdint.h>
#include <cputables.h>

typedef enum
{
    ISR_DivideByZero = 0x0,
    ISR_Debug = 0x1,
    ISR_NonMaskableInterrupt = 0x2,
    ISR_Breakpoint = 0x3,
    ISR_Overflow = 0x4,
    ISR_BoundRange = 0x5,
    ISR_InvalidOpcode = 0x6,
    ISR_DeviceNotAvailable = 0x7,
    ISR_DoubleFault = 0x8,
    ISR_CoprocessorSegmentOverrun = 0x9,
    ISR_InvalidTSS = 0xa,
    ISR_SegmentNotPresent = 0xb,
    ISR_StackSegmentFault = 0xc,
    ISR_GeneralProtectionFault = 0xd,
    ISR_PageFault = 0xe,
    ISR_x87FloatingPoint = 0x10,
    ISR_AlignmentCheck = 0x11,
    ISR_MachineCheck = 0x12,
    ISR_SIMDFloatingPoint = 0x13,
    ISR_Virtualization = 0x14,
    ISR_Security = 0x1e
} ISRExceptions;

typedef enum
{
    ISR0 = 0x0,   // Divide-by-zero Error
    ISR1 = 0x1,   // Debug
    ISR2 = 0x2,   // Non-maskable Interrupt
    ISR3 = 0x3,   // Breakpoint
    ISR4 = 0x4,   // Overflow
    ISR5 = 0x5,   // Bound Range Exceeded
    ISR6 = 0x6,   // Invalid Opcode
    ISR7 = 0x7,   // Device Not Available
    ISR8 = 0x8,   // Double Fault
    ISR9 = 0x9,   // Coprocessor Segment Overrun
    ISR10 = 0xa,  // Invalid TSS
    ISR11 = 0xb,  // Segment Not P
    ISR12 = 0xc,  // Stack-Segment Fault
    ISR13 = 0xd,  // General Protection Fault
    ISR14 = 0xe,  // Page Fault
    ISR15 = 0xf,  // Reserved
    ISR16 = 0x10, // x87 Floating-Point Exception
    ISR17 = 0x11, // Alignment Check
    ISR18 = 0x12, // Machine Check
    ISR19 = 0x13, // SIMD Floating-Point Exception
    ISR20 = 0x14, // Virtualization Exception
    ISR21 = 0x15, // Reserved
    ISR22 = 0x16, // Reserved
    ISR23 = 0x17, // Reserved
    ISR24 = 0x18, // Reserved
    ISR25 = 0x19, // Reserved
    ISR26 = 0x1a, // Reserved
    ISR27 = 0x1b, // Reserved
    ISR28 = 0x1c, // Reserved
    ISR29 = 0x1d, // Reserved
    ISR30 = 0x1e, // Security Exception
    ISR31 = 0x1f, // Reserved

    IRQ0 = 0x20,  //	Programmable Interrupt Timer Interrupt
    IRQ1 = 0x21,  //	Keyboard Interrupt
    IRQ2 = 0x22,  //	Cascade (used internally by the two PICs. never raised)
    IRQ3 = 0x23,  //	COM2 (if enabled)
    IRQ4 = 0x24,  //	COM1 (if enabled)
    IRQ5 = 0x25,  //	LPT2 (if enabled)
    IRQ6 = 0x26,  //	Floppy Disk
    IRQ7 = 0x27,  //	LPT1 / Unreliable "spurious" interrupt (usually)
    IRQ8 = 0x28,  //	CMOS real-time clock (if enabled)
    IRQ9 = 0x29,  //	Free for peripherals / legacy SCSI / NIC
    IRQ10 = 0x2a, //	Free for peripherals / SCSI / NIC
    IRQ11 = 0x2b, //	Free for peripherals / SCSI / NIC
    IRQ12 = 0x2c, //	PS2 Mouse
    IRQ13 = 0x2d, //	FPU / Coprocessor / Inter-processor
    IRQ14 = 0x2e, //	Primary ATA Hard Disk
    IRQ15 = 0x2f, //	Secondary ATA Hard Disk

    IRQ16 = 0x30, //    Reserved for multitasking
    IRQ17 = 0x31, //    Reserved for monotasking

    IRQ18 = 0x32,
    IRQ19 = 0x33,
    IRQ20 = 0x34,
    IRQ21 = 0x35,
    IRQ22 = 0x36,
    IRQ23 = 0x37,
    IRQ24 = 0x38,
    IRQ25 = 0x39,
    IRQ26 = 0x3a,
    IRQ27 = 0x3b,
    IRQ28 = 0x3c,
    IRQ29 = 0x3d,
    IRQ30 = 0x3e,
    IRQ31 = 0x3f,
    IRQ32 = 0x40,
    IRQ33 = 0x41,
    IRQ34 = 0x42,
    IRQ35 = 0x43,
    IRQ36 = 0x44,
    IRQ37 = 0x45,
    IRQ38 = 0x46,
    IRQ39 = 0x47,
    IRQ40 = 0x48,
    IRQ41 = 0x49,
    IRQ42 = 0x4a,
    IRQ43 = 0x4b,
    IRQ44 = 0x4c,
    IRQ45 = 0x4d,
    IRQ46 = 0x4e,
    IRQ47 = 0x4f,
    IRQ48 = 0x50,
    IRQ49 = 0x51,
    IRQ50 = 0x52,
    IRQ51 = 0x53,
    IRQ52 = 0x54,
    IRQ53 = 0x55,
    IRQ54 = 0x56,
    IRQ55 = 0x57,
    IRQ56 = 0x58,
    IRQ57 = 0x59,
    IRQ58 = 0x5a,
    IRQ59 = 0x5b,
    IRQ60 = 0x5c,
    IRQ61 = 0x5d,
    IRQ62 = 0x5e,
    IRQ63 = 0x5f,
    IRQ64 = 0x60,
    IRQ65 = 0x61,
    IRQ66 = 0x62,
    IRQ67 = 0x63,
    IRQ68 = 0x64,
    IRQ69 = 0x65,
    IRQ70 = 0x66,
    IRQ71 = 0x67,
    IRQ72 = 0x68,
    IRQ73 = 0x69,
    IRQ74 = 0x6a,
    IRQ75 = 0x6b,
    IRQ76 = 0x6c,
    IRQ77 = 0x6d,
    IRQ78 = 0x6e,
    IRQ79 = 0x6f,
    IRQ80 = 0x70,
    IRQ81 = 0x71,
    IRQ82 = 0x72,
    IRQ83 = 0x73,
    IRQ84 = 0x74,
    IRQ85 = 0x75,
    IRQ86 = 0x76,
    IRQ87 = 0x77,
    IRQ88 = 0x78,
    IRQ89 = 0x79,
    IRQ90 = 0x7a,
    IRQ91 = 0x7b,
    IRQ92 = 0x7c,
    IRQ93 = 0x7d,
    IRQ94 = 0x7e,
    IRQ95 = 0x7f,
    IRQ96 = 0x80,
    IRQ97 = 0x81,
    IRQ98 = 0x82,
    IRQ99 = 0x83,
    IRQ100 = 0x84,
    IRQ101 = 0x85,
    IRQ102 = 0x86,
    IRQ103 = 0x87,
    IRQ104 = 0x88,
    IRQ105 = 0x89,
    IRQ106 = 0x8a,
    IRQ107 = 0x8b,
    IRQ108 = 0x8c,
    IRQ109 = 0x8d,
    IRQ110 = 0x8e,
    IRQ111 = 0x8f,
    IRQ112 = 0x90,
    IRQ113 = 0x91,
    IRQ114 = 0x92,
    IRQ115 = 0x93,
    IRQ116 = 0x94,
    IRQ117 = 0x95,
    IRQ118 = 0x96,
    IRQ119 = 0x97,
    IRQ120 = 0x98,
    IRQ121 = 0x99,
    IRQ122 = 0x9a,
    IRQ123 = 0x9b,
    IRQ124 = 0x9c,
    IRQ125 = 0x9d,
    IRQ126 = 0x9e,
    IRQ127 = 0x9f,
    IRQ128 = 0xa0,
    IRQ129 = 0xa1,
    IRQ130 = 0xa2,
    IRQ131 = 0xa3,
    IRQ132 = 0xa4,
    IRQ133 = 0xa5,
    IRQ134 = 0xa6,
    IRQ135 = 0xa7,
    IRQ136 = 0xa8,
    IRQ137 = 0xa9,
    IRQ138 = 0xaa,
    IRQ139 = 0xab,
    IRQ140 = 0xac,
    IRQ141 = 0xad,
    IRQ142 = 0xae,
    IRQ143 = 0xaf,
    IRQ144 = 0xb0,
    IRQ145 = 0xb1,
    IRQ146 = 0xb2,
    IRQ147 = 0xb3,
    IRQ148 = 0xb4,
    IRQ149 = 0xb5,
    IRQ150 = 0xb6,
    IRQ151 = 0xb7,
    IRQ152 = 0xb8,
    IRQ153 = 0xb9,
    IRQ154 = 0xba,
    IRQ155 = 0xbb,
    IRQ156 = 0xbc,
    IRQ157 = 0xbd,
    IRQ158 = 0xbe,
    IRQ159 = 0xbf,
    IRQ160 = 0xc0,
    IRQ161 = 0xc1,
    IRQ162 = 0xc2,
    IRQ163 = 0xc3,
    IRQ164 = 0xc4,
    IRQ165 = 0xc5,
    IRQ166 = 0xc6,
    IRQ167 = 0xc7,
    IRQ168 = 0xc8,
    IRQ169 = 0xc9,
    IRQ170 = 0xca,
    IRQ171 = 0xcb,
    IRQ172 = 0xcc,
    IRQ173 = 0xcd,
    IRQ174 = 0xce,
    IRQ175 = 0xcf,
    IRQ176 = 0xd0,
    IRQ177 = 0xd1,
    IRQ178 = 0xd2,
    IRQ179 = 0xd3,
    IRQ180 = 0xd4,
    IRQ181 = 0xd5,
    IRQ182 = 0xd6,
    IRQ183 = 0xd7,
    IRQ184 = 0xd8,
    IRQ185 = 0xd9,
    IRQ186 = 0xda,
    IRQ187 = 0xdb,
    IRQ188 = 0xdc,
    IRQ189 = 0xdd,
    IRQ190 = 0xde,
    IRQ191 = 0xdf,
    IRQ192 = 0xe0,
    IRQ193 = 0xe1,
    IRQ194 = 0xe2,
    IRQ195 = 0xe3,
    IRQ196 = 0xe4,
    IRQ197 = 0xe5,
    IRQ198 = 0xe6,
    IRQ199 = 0xe7,
    IRQ200 = 0xe8,
    IRQ201 = 0xe9,
    IRQ202 = 0xea,
    IRQ203 = 0xeb,
    IRQ204 = 0xec,
    IRQ205 = 0xed,
    IRQ206 = 0xee,
    IRQ207 = 0xef,
    IRQ208 = 0xf0,
    IRQ209 = 0xf1,
    IRQ210 = 0xf2,
    IRQ211 = 0xf3,
    IRQ212 = 0xf4,
    IRQ213 = 0xf5,
    IRQ214 = 0xf6,
    IRQ215 = 0xf7,
    IRQ216 = 0xf8,
    IRQ217 = 0xf9,
    IRQ218 = 0xfa,
    IRQ219 = 0xfb,
    IRQ220 = 0xfc,
    IRQ221 = 0xfd,
    IRQ222 = 0xfe,
    IRQ223 = 0xff,
} Interrupts;

typedef struct _TrapFrame
{
    // uint64_t gs;  // General-purpose Segment
    // uint64_t fs;  // General-purpose Segment
    // uint64_t es;  // Extra Segment (used for string operations)
    // uint64_t ds;  // Data Segment

    uint64_t r15; // General purpose
    uint64_t r14; // General purpose
    uint64_t r13; // General purpose
    uint64_t r12; // General purpose
    uint64_t r11; // General purpose
    uint64_t r10; // General purpose
    uint64_t r9;  // General purpose
    uint64_t r8;  // General purpose

    uint64_t rbp; // Base Pointer (meant for stack frames)
    uint64_t rdi; // Destination index for string operations
    uint64_t rsi; // Source index for string operations
    uint64_t rdx; // Data (commonly extends the A register)
    uint64_t rcx; // Counter
    uint64_t rbx; // Base
    uint64_t rax; // Accumulator

    uint64_t int_num;    // Interrupt Number
    uint64_t error_code; // Error code
    uint64_t rip;        // Instruction Pointer
    uint64_t cs;         // Code Segment
    RFLAGS rflags;       // Register Flags
    uint64_t rsp;        // Stack Pointer
    uint64_t ss;         // Stack Segment
} TrapFrame;

#define FUNCTION rip
#define ARG0 rdi
#define ARG1 rsi
#define STACK rsp

#define ES regs->es                 // Extra Segment (used for string operations)
#define FS regs->fs                 // General-purpose Segment
#define GS regs->gs                 // General-purpose Segment
#define DS regs->r15                // Data Segment
#define R15 regs->r15               // General purpose
#define R14 regs->r14               // General purpose
#define R13 regs->r13               // General purpose
#define R12 regs->r12               // General purpose
#define R11 regs->r11               // General purpose
#define R10 regs->r10               // General purpose
#define R9 regs->r9                 // General purpose
#define R8 regs->r8                 // General purpose
#define RBP regs->rbp               // Base Pointer (meant for stack frames)
#define RDI regs->rdi               // Destination index for string operations
#define RSI regs->rsi               // Source index for string operations
#define RDX regs->rdx               // Data (commonly extends the A register)
#define RCX regs->rcx               // Counter
#define RBX regs->rbx               // Base
#define RAX regs->rax               // Accumulator
#define INT_NUM regs->int_num       // Interrupt Number
#define ERROR_CODE regs->error_code // Error code
#define RIP regs->rip               // Instruction Pointer
#define CS regs->cs                 // Code Segment
#define FLAGS regs->rflags          // Register Flags
#define RSP regs->rsp               // Stack Pointer
#define _SS regs->ss                // Stack Segment

#define InterruptHandler(name) static void name(TrapFrame *regs)
