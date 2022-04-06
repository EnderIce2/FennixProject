#ifndef __FENNIX_KERNEL_INTERRUPTS_H__
#define __FENNIX_KERNEL_INTERRUPTS_H__

#include <types.h>

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

#define ISR0 0x00  // Divide-by-zero Error
#define ISR1 0x01  // Debug
#define ISR2 0x02  // Non-maskable Interrupt
#define ISR3 0x03  // Breakpoint
#define ISR4 0x04  // Overflow
#define ISR5 0x05  // Bound Range Exceeded
#define ISR6 0x06  // Invalid Opcode
#define ISR7 0x07  // Device Not Available
#define ISR8 0x08  // Double Fault
#define ISR9 0x09  // Coprocessor Segment Overrun
#define ISR10 0x0a // Invalid TSS
#define ISR11 0x0b // Segment Not P
#define ISR12 0x0c // Stack-Segment Fault
#define ISR13 0x0d // General Protection Fault
#define ISR14 0x0e // Page Fault
#define ISR15 0x0f // Reserved
#define ISR16 0x10 // x87 Floating-Point Exception
#define ISR17 0x11 // Alignment Check
#define ISR18 0x12 // Machine Check
#define ISR19 0x13 // SIMD Floating-Point Exception
#define ISR20 0x14 // Virtualization Exception
#define ISR21 0x15 // Reserved
#define ISR22 0x16 // Reserved
#define ISR23 0x17 // Reserved
#define ISR24 0x18 // Reserved
#define ISR25 0x19 // Reserved
#define ISR26 0x1a // Reserved
#define ISR27 0x1b // Reserved
#define ISR28 0x1c // Reserved
#define ISR29 0x1d // Reserved
#define ISR30 0x1e // Security Exception
#define ISR31 0x1f // Reserved

#define IRQ0 0x20  //	Programmable Interrupt Timer Interrupt
#define IRQ1 0x21  //	Keyboard Interrupt
#define IRQ2 0x22  //	Cascade (used internally by the two PICs. never raised)
#define IRQ3 0x23  //	COM2 (if enabled)
#define IRQ4 0x24  //	COM1 (if enabled)
#define IRQ5 0x25  //	LPT2 (if enabled)
#define IRQ6 0x26  //	Floppy Disk
#define IRQ7 0x27  //	LPT1 / Unreliable "spurious" interrupt (usually)
#define IRQ8 0x28  //	CMOS real-time clock (if enabled)
#define IRQ9 0x29  //	Free for peripherals / legacy SCSI / NIC
#define IRQ10 0x2a //	Free for peripherals / SCSI / NIC
#define IRQ11 0x2b //	Free for peripherals / SCSI / NIC
#define IRQ12 0x2c //	PS2 Mouse
#define IRQ13 0x2d //	FPU / Coprocessor / Inter-processor
#define IRQ14 0x2e //	Primary ATA Hard Disk
#define IRQ15 0x2f //	Secondary ATA Hard Disk

#ifdef __x86_64__

typedef union
{
    struct
    {
        uint32_t CF : 1;          // Carry Flag
        uint32_t always_one : 1;  // Reserved
        uint32_t PF : 1;          // Parity Flag
        uint32_t reserved_1 : 1;  // Reserved
        uint32_t AF : 1;          // Auxiliary Carry Flag
        uint32_t reserved_2 : 1;  // Reserved
        uint32_t ZF : 1;          // Zero Flag
        uint32_t SF : 1;          // Sign Flag
        uint32_t TF : 1;          // Trap Flag
        uint32_t IF : 1;          // Interrupt Enable Flag
        uint32_t DF : 1;          // Direction Flag
        uint32_t OF : 1;          // Overflow Flag
        uint32_t IOPL : 2;        // I/O Privilege Level
        uint32_t NT : 1;          // Nested Task
        uint32_t reserved_3 : 1;  // Reserved
        uint32_t RF : 1;          // Resume Flag
        uint32_t VM : 1;          // Virtual 8086 Mode
        uint32_t AC : 1;          // Alignment Check
        uint32_t VIF : 1;         // Virtual Interrupt Flag
        uint32_t VIP : 1;         // Virtual Interrupt Pending
        uint32_t ID : 1;          // ID Flag
        uint32_t reserved_4 : 10; // Reserved
    };
    uint64_t raw;
} RFLAGS;

typedef struct _REGISTERS
{
    // uint64_t es;         // Extra Segment (used for string operations)
    // uint64_t fs;         // General-purpose Segment
    // uint64_t gs;         // General-purpose Segment
    uint64_t ds;         // Data Segment
    uint64_t r15;        // General purpose
    uint64_t r14;        // General purpose
    uint64_t r13;        // General purpose
    uint64_t r12;        // General purpose
    uint64_t r11;        // General purpose
    uint64_t r10;        // General purpose
    uint64_t r9;         // General purpose
    uint64_t r8;         // General purpose
    uint64_t rbp;        // Base Pointer (meant for stack frames)
    uint64_t rdi;        // Destination index for string operations
    uint64_t rsi;        // Source index for string operations
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
    uint64_t ss;         // Stack Segment
#define FUNCTION rip
#define ARG0 rdi
#define ARG1 rsi
#define STACK rsp
} REGISTERS;

#endif

#define ES regs->es                 // Extra Segment (used for string operations)
#define FS regs->fs                 // General-purpose Segment
#define GS regs->gs                 // General-purpose Segment
#define DS regs->ds                 // Data Segment
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
#define SS regs->ss                 // Stack Segment

EXTERNC void EndOfInterrupt(int interrupt);

#define InterruptHandler(name) static void name(REGISTERS *regs)

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20 /* End-of-interrupt command code */

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

void PIC_disable();
void PIC_sendEOI(unsigned char irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);

#endif // !__FENNIX_KERNEL_INTERRUPTS_H__
