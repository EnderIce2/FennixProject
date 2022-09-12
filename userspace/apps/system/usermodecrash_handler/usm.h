#pragma once

typedef union
{
    struct
    {
        /** @brief Carry Flag */
        __UINT64_TYPE__ CF : 1;
        /** @brief Reserved */
        __UINT64_TYPE__ always_one : 1;
        /** @brief Parity Flag */
        __UINT64_TYPE__ PF : 1;
        /** @brief Reserved */
        __UINT64_TYPE__ _reserved0 : 1;
        /** @brief Auxiliary Carry Flag */
        __UINT64_TYPE__ AF : 1;
        /** @brief Reserved */
        __UINT64_TYPE__ _reserved1 : 1;
        /** @brief Zero Flag */
        __UINT64_TYPE__ ZF : 1;
        /** @brief Sign Flag */
        __UINT64_TYPE__ SF : 1;
        /** @brief Trap Flag */
        __UINT64_TYPE__ TF : 1;
        /** @brief Interrupt Enable Flag */
        __UINT64_TYPE__ IF : 1;
        /** @brief Direction Flag */
        __UINT64_TYPE__ DF : 1;
        /** @brief Overflow Flag */
        __UINT64_TYPE__ OF : 1;
        /** @brief I/O Privilege Level */
        __UINT64_TYPE__ IOPL : 2;
        /** @brief Nested Task */
        __UINT64_TYPE__ NT : 1;
        /** @brief Reserved */
        __UINT64_TYPE__ _reserved2 : 1;
        /** @brief Resume Flag */
        __UINT64_TYPE__ RF : 1;
        /** @brief Virtual 8086 Mode */
        __UINT64_TYPE__ VM : 1;
        /** @brief Alignment Check */
        __UINT64_TYPE__ AC : 1;
        /** @brief Virtual Interrupt Flag */
        __UINT64_TYPE__ VIF : 1;
        /** @brief Virtual Interrupt Pending */
        __UINT64_TYPE__ VIP : 1;
        /** @brief ID Flag */
        __UINT64_TYPE__ ID : 1;
        /** @brief Reserved */
        __UINT64_TYPE__ _reserved3 : 10;
    };
    __UINT64_TYPE__ raw;
} RFLAGS;

typedef struct _TrapFrame
{
    // __UINT64_TYPE__ gs;  // General-purpose Segment
    // __UINT64_TYPE__ fs;  // General-purpose Segment
    // __UINT64_TYPE__ es;  // Extra Segment (used for string operations)
    // __UINT64_TYPE__ ds;  // Data Segment

    __UINT64_TYPE__ r15; // General purpose
    __UINT64_TYPE__ r14; // General purpose
    __UINT64_TYPE__ r13; // General purpose
    __UINT64_TYPE__ r12; // General purpose
    __UINT64_TYPE__ r11; // General purpose
    __UINT64_TYPE__ r10; // General purpose
    __UINT64_TYPE__ r9;  // General purpose
    __UINT64_TYPE__ r8;  // General purpose

    __UINT64_TYPE__ rbp; // Base Pointer (meant for stack frames)
    __UINT64_TYPE__ rdi; // Destination index for string operations
    __UINT64_TYPE__ rsi; // Source index for string operations
    __UINT64_TYPE__ rdx; // Data (commonly extends the A register)
    __UINT64_TYPE__ rcx; // Counter
    __UINT64_TYPE__ rbx; // Base
    __UINT64_TYPE__ rax; // Accumulator

    __UINT64_TYPE__ int_num;    // Interrupt Number
    __UINT64_TYPE__ error_code; // Error code
    __UINT64_TYPE__ rip;        // Instruction Pointer
    __UINT64_TYPE__ cs;         // Code Segment
    RFLAGS rflags;       // Register Flags
    __UINT64_TYPE__ rsp;        // Stack Pointer
    __UINT64_TYPE__ ss;         // Stack Segment
} TrapFrame;

#define FUNCTION rip
#define ARG0 rdi
#define ARG1 rsi
#define STACK rsp
