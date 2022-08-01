#include "idt.h"

#include "../interrupts/pic.h"
#include "../drivers/serial.h"
#include "gdt.h"

#include <interrupts.h>
#include <boot/gbp.h>
#include <display.h>
#include <int.h>
#include <sys.h>
#include <asm.h>
#include <io.h>

void *KPML4Address = NULL;

void SetKernelPageTableAddress(void *Address)
{
    KPML4Address = Address;
    trace("Kernel page table address set to %p", KPML4Address);
}

__attribute__((naked, used, no_stack_protector)) void exception_handler_helper()
{
    // TODO: Switching page table if ring 0 is not tested! (source: https://www.tutorialspoint.com/assembly_programming/assembly_conditions.htm)
    asm("cld\n" // clear direction flag

        "pushq %rax\n"    // push rax
        "mov %cs, %rax\n" // move cs to rax

        "cmp %rax, [0x8]\n"        // compare rax with 0x8
        "jne .NoPageTableUpdate\n" // if not, skip to next instruction
        "push %rax\n");            // push rax

    // "mov $0x100000, %rax\n"    // set rax to kernel pml4 of memory
    asm volatile("mov %[KPML4Address], %%rax" /* Not sure if it will work but I didn't had any issues. */
                 :
                 : [KPML4Address] "q"(KPML4Address)
                 : "memory");

    asm("mov %rax, %cr3\n"      // set page directory
        "pop %rax\n"            // pop rax
        ".NoPageTableUpdate:\n" // label for jumping to next instruction
        "popq %rax\n"           // pop rax

        // push all registers
        "pushq %rax\n"
        "pushq %rbx\n"
        "pushq %rcx\n"
        "pushq %rdx\n"
        "pushq %rsi\n"
        "pushq %rdi\n"
        "pushq %rbp\n"
        "pushq %r8\n"
        "pushq %r9\n"
        "pushq %r10\n"
        "pushq %r11\n"
        "pushq %r12\n"
        "pushq %r13\n"
        "pushq %r14\n"
        "pushq %r15\n"

        "movq %rsp, %rdi\n"
        "call exception_handler\n"

        // pop all registers
        "popq %r15\n"
        "popq %r14\n"
        "popq %r13\n"
        "popq %r12\n"
        "popq %r11\n"
        "popq %r10\n"
        "popq %r9\n"
        "popq %r8\n"
        "popq %rbp\n"
        "popq %rdi\n"
        "popq %rsi\n"
        "popq %rdx\n"
        "popq %rcx\n"
        "popq %rbx\n"
        "popq %rax\n"

        "addq $16, %rsp\n"
        "iretq"); // pop CS RIP RFLAGS SS ESP
}

__attribute__((used)) void exception_handler(TrapFrame *regs)
{
    CLI;
    serial_write_text(COM1, "An Internal Exception Occurred\n");
    // serial_write_text(COM1, to_char(INT_NUM));
    // serial_write_text(COM1, "\n");
    switch (INT_NUM)
    {
    case ISR8:
        serial_write_text(COM1, "DOUBLE FAULT!\n");
#ifndef DEBUG
        uint32_t Divd;
        uint8_t tmpd;
        Divd = 1193180 / 690;
        outb(0x43, 0xb6);
        outb(0x42, (uint8_t)(Divd));
        outb(0x42, (uint8_t)(Divd >> 8));
        tmpd = inb(0x61);
        if (tmpd != (tmpd | 3))
        {
            outb(0x61, tmpd | 3);
        }
#endif
        struct Framebuffer *fb = GetFramebuffer();
        for (uint32_t VerticalScanline = 0; VerticalScanline < fb->Height; VerticalScanline++)
        {
            uint64_t PixelPtrBase = fb->Address + ((fb->PixelsPerScanLine * 4) * VerticalScanline);
            for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (fb->PixelsPerScanLine * 4)); PixelPtr++)
            {
                *PixelPtr = 0xFF6F0000;
            }
        }
        CPU_HALT;
        return;
    default:
        if (INT_NUM <= ISR31)
        {
            TRACEREGS(regs);
            err("Fatal error detected (%d)", INT_NUM);
            goto exception_handler_;
        }
        err("Unknown interrupt %d", INT_NUM);
        return;
    }
exception_handler_:
#ifndef DEBUG
    uint32_t Div;
    uint8_t tmp;
    Div = 1193180 / 1000;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(Div));
    outb(0x42, (uint8_t)(Div >> 8));
    tmp = inb(0x61);
    if (tmp != (tmp | 3))
        outb(0x61, tmp | 3);
    STI;
    msleep(10);
    CLI;
    uint8_t tmp1 = inb(0x61) & 0xFC;
    outb(0x61, tmp1);
#endif
    isrcrash(regs);
    CPU_HALT;
}

#define EXCEPTION_HANDLER(num)                                                       \
    __attribute__((naked, no_stack_protector)) static void interrupt_handler_##num() \
    {                                                                                \
        asm("pushq $0\npushq $" #num "\n"                                            \
            "jmp exception_handler_helper");                                         \
    }

#define EXCEPTION_ERROR_HANDLER(num)                             \
    __attribute__((naked)) static void interrupt_handler_##num() \
    {                                                            \
        asm("pushq $" #num "\n"                                  \
            "jmp exception_handler_helper");                     \
    }

/* =============================================================================================================================================== */

__attribute__((naked, used, no_stack_protector)) static void InterruptHandlerStub()
{
    asm("cld\n"
        "pushq %rax\n"
        "pushq %rbx\n"
        "pushq %rcx\n"
        "pushq %rdx\n"
        "pushq %rsi\n"
        "pushq %rdi\n"
        "pushq %rbp\n"
        "pushq %r8\n"
        "pushq %r9\n"
        "pushq %r10\n"
        "pushq %r11\n"
        "pushq %r12\n"
        "pushq %r13\n"
        "pushq %r14\n"
        "pushq %r15\n"

        "mov %rsp, %rdi\n"
        "call IDTInterruptHandler\n"

        "popq %r15\n"
        "popq %r14\n"
        "popq %r13\n"
        "popq %r12\n"
        "popq %r11\n"
        "popq %r10\n"
        "popq %r9\n"
        "popq %r8\n"
        "popq %rbp\n"
        "popq %rdi\n"
        "popq %rsi\n"
        "popq %rdx\n"
        "popq %rcx\n"
        "popq %rbx\n"
        "popq %rax\n"

        "addq $16, %rsp\n"
        "iretq");
}

#define INTERRUPT_HANDLER(num)                                                      \
    __attribute__((naked, used, no_stack_protector)) void interrupt_handler_##num() \
    {                                                                               \
        asm("pushq $0\npushq $" #num "\n"                                           \
            "jmp InterruptHandlerStub\n");                                          \
    }

INTERRUPT_HANDLER interrupt_handlers[256];

static void IDTInterruptHandler(TrapFrame *regs)
{
    if (((long)((int32_t)regs->int_num)) < 0 || regs->int_num > 0xff)
    {
        err("Invalid interrupt received %#llx", regs->int_num);
    }
    else if (interrupt_handlers[regs->int_num] == NULL)
    {
        err("IRQ%d is not registered!", regs->int_num - 32);
    }
    else
        interrupt_handlers[regs->int_num](regs);
    EndOfInterrupt(regs->int_num);
}

void register_interrupt_handler(uint8_t vector, INTERRUPT_HANDLER handle)
{
    interrupt_handlers[vector] = handle;
    debug("Vector %#llx(IRQ%d) has been registered to handle %#llx", vector, vector - 32, handle);
}

void unregister_interrupt_handler(uint8_t vector)
{
    interrupt_handlers[vector] = NULL;
    debug("Vector %#llx(IRQ%d) has been unregistered", vector, vector - 32);
}

/* =============================================================================================================================================== */

static InterruptDescriptorTableEntry idt_entries[0x100];

InterruptDescriptorTableDescriptor idtr = {.Length = sizeof(idt_entries) - 1,
                                           .Entries = idt_entries};

void set_idt_entry(uint8_t idt, void (*handler)(), uint64_t ist, uint64_t ring)
{
    idt_entries[idt].BaseLow = (uint16_t)((uint64_t)handler & 0xFFFF);
    idt_entries[idt].BaseHigh = (uint64_t)((uint64_t)handler >> 16);
    idt_entries[idt].Flags = FlagGate32BIT_TRAP;
    idt_entries[idt].SegmentSelector = GDT_KERNEL_CODE;
    idt_entries[idt].Present = 1;
    idt_entries[idt].InterruptStackTable = ist;
    idt_entries[idt].Ring = ring;
}

/* =============================================================================================================================================== */

// ISR
EXCEPTION_HANDLER(0x00);
EXCEPTION_HANDLER(0x01);
EXCEPTION_HANDLER(0x02);
EXCEPTION_HANDLER(0x03);
EXCEPTION_HANDLER(0x04);
EXCEPTION_HANDLER(0x05);
EXCEPTION_HANDLER(0x06);
EXCEPTION_HANDLER(0x07);
EXCEPTION_ERROR_HANDLER(0x08);
EXCEPTION_HANDLER(0x09);
EXCEPTION_ERROR_HANDLER(0x0a);
EXCEPTION_ERROR_HANDLER(0x0b);
EXCEPTION_ERROR_HANDLER(0x0c);
EXCEPTION_ERROR_HANDLER(0x0d);
EXCEPTION_ERROR_HANDLER(0x0e);
EXCEPTION_HANDLER(0x0f);
EXCEPTION_ERROR_HANDLER(0x10);
EXCEPTION_HANDLER(0x11);
EXCEPTION_HANDLER(0x12);
EXCEPTION_HANDLER(0x13);
EXCEPTION_HANDLER(0x14);
EXCEPTION_HANDLER(0x15);
EXCEPTION_HANDLER(0x16);
EXCEPTION_HANDLER(0x17);
EXCEPTION_HANDLER(0x18);
EXCEPTION_HANDLER(0x19);
EXCEPTION_HANDLER(0x1a);
EXCEPTION_HANDLER(0x1b);
EXCEPTION_HANDLER(0x1c);
EXCEPTION_HANDLER(0x1d);
EXCEPTION_HANDLER(0x1e);
EXCEPTION_HANDLER(0x1f);

// IRQ
INTERRUPT_HANDLER(0x20)
INTERRUPT_HANDLER(0x21)
INTERRUPT_HANDLER(0x22)
INTERRUPT_HANDLER(0x23)
INTERRUPT_HANDLER(0x24)
INTERRUPT_HANDLER(0x25)
INTERRUPT_HANDLER(0x26)
INTERRUPT_HANDLER(0x27)
INTERRUPT_HANDLER(0x28)
INTERRUPT_HANDLER(0x29)
INTERRUPT_HANDLER(0x2a)
INTERRUPT_HANDLER(0x2b)
INTERRUPT_HANDLER(0x2c)
INTERRUPT_HANDLER(0x2d)
INTERRUPT_HANDLER(0x2e)
INTERRUPT_HANDLER(0x2f)

__attribute__((naked, used, no_stack_protector)) void interrupt_handler_0x30() { asm("pushq $0\npushq $0x30\n"
                                                                                     "jmp MultiTaskingSchedulerHelper"); }
__attribute__((naked, used, no_stack_protector)) void interrupt_handler_0x31() { asm("pushq $0\npushq $0x31\n"
                                                                                     "jmp MonoTaskingSchedulerHelper"); }
INTERRUPT_HANDLER(0x32)
INTERRUPT_HANDLER(0x33)
INTERRUPT_HANDLER(0x34)
INTERRUPT_HANDLER(0x35)
INTERRUPT_HANDLER(0x36)
INTERRUPT_HANDLER(0x37)
INTERRUPT_HANDLER(0x38)
INTERRUPT_HANDLER(0x39)
INTERRUPT_HANDLER(0x3a)
INTERRUPT_HANDLER(0x3b)
INTERRUPT_HANDLER(0x3c)
INTERRUPT_HANDLER(0x3d)
INTERRUPT_HANDLER(0x3e)
INTERRUPT_HANDLER(0x3f)
INTERRUPT_HANDLER(0x40)
INTERRUPT_HANDLER(0x41)
INTERRUPT_HANDLER(0x42)
INTERRUPT_HANDLER(0x43)
INTERRUPT_HANDLER(0x44)
INTERRUPT_HANDLER(0x45)
INTERRUPT_HANDLER(0x46)
INTERRUPT_HANDLER(0x47)
INTERRUPT_HANDLER(0x48)
INTERRUPT_HANDLER(0x49)
INTERRUPT_HANDLER(0x4a)
INTERRUPT_HANDLER(0x4b)
INTERRUPT_HANDLER(0x4c)
INTERRUPT_HANDLER(0x4d)
INTERRUPT_HANDLER(0x4e)
INTERRUPT_HANDLER(0x4f)
INTERRUPT_HANDLER(0x50)
INTERRUPT_HANDLER(0x51)
INTERRUPT_HANDLER(0x52)
INTERRUPT_HANDLER(0x53)
INTERRUPT_HANDLER(0x54)
INTERRUPT_HANDLER(0x55)
INTERRUPT_HANDLER(0x56)
INTERRUPT_HANDLER(0x57)
INTERRUPT_HANDLER(0x58)
INTERRUPT_HANDLER(0x59)
INTERRUPT_HANDLER(0x5a)
INTERRUPT_HANDLER(0x5b)
INTERRUPT_HANDLER(0x5c)
INTERRUPT_HANDLER(0x5d)
INTERRUPT_HANDLER(0x5e)
INTERRUPT_HANDLER(0x5f)
INTERRUPT_HANDLER(0x60)
INTERRUPT_HANDLER(0x61)
INTERRUPT_HANDLER(0x62)
INTERRUPT_HANDLER(0x63)
INTERRUPT_HANDLER(0x64)
INTERRUPT_HANDLER(0x65)
INTERRUPT_HANDLER(0x66)
INTERRUPT_HANDLER(0x67)
INTERRUPT_HANDLER(0x68)
INTERRUPT_HANDLER(0x69)
INTERRUPT_HANDLER(0x6a)
INTERRUPT_HANDLER(0x6b)
INTERRUPT_HANDLER(0x6c)
INTERRUPT_HANDLER(0x6d)
INTERRUPT_HANDLER(0x6e)
INTERRUPT_HANDLER(0x6f)
INTERRUPT_HANDLER(0x70)
INTERRUPT_HANDLER(0x71)
INTERRUPT_HANDLER(0x72)
INTERRUPT_HANDLER(0x73)
INTERRUPT_HANDLER(0x74)
INTERRUPT_HANDLER(0x75)
INTERRUPT_HANDLER(0x76)
INTERRUPT_HANDLER(0x77)
INTERRUPT_HANDLER(0x78)
INTERRUPT_HANDLER(0x79)
INTERRUPT_HANDLER(0x7a)
INTERRUPT_HANDLER(0x7b)
INTERRUPT_HANDLER(0x7c)
INTERRUPT_HANDLER(0x7d)
INTERRUPT_HANDLER(0x7e)
INTERRUPT_HANDLER(0x7f)
INTERRUPT_HANDLER(0x80)
INTERRUPT_HANDLER(0x81)
INTERRUPT_HANDLER(0x82)
INTERRUPT_HANDLER(0x83)
INTERRUPT_HANDLER(0x84)
INTERRUPT_HANDLER(0x85)
INTERRUPT_HANDLER(0x86)
INTERRUPT_HANDLER(0x87)
INTERRUPT_HANDLER(0x88)
INTERRUPT_HANDLER(0x89)
INTERRUPT_HANDLER(0x8a)
INTERRUPT_HANDLER(0x8b)
INTERRUPT_HANDLER(0x8c)
INTERRUPT_HANDLER(0x8d)
INTERRUPT_HANDLER(0x8e)
INTERRUPT_HANDLER(0x8f)
INTERRUPT_HANDLER(0x90)
INTERRUPT_HANDLER(0x91)
INTERRUPT_HANDLER(0x92)
INTERRUPT_HANDLER(0x93)
INTERRUPT_HANDLER(0x94)
INTERRUPT_HANDLER(0x95)
INTERRUPT_HANDLER(0x96)
INTERRUPT_HANDLER(0x97)
INTERRUPT_HANDLER(0x98)
INTERRUPT_HANDLER(0x99)
INTERRUPT_HANDLER(0x9a)
INTERRUPT_HANDLER(0x9b)
INTERRUPT_HANDLER(0x9c)
INTERRUPT_HANDLER(0x9d)
INTERRUPT_HANDLER(0x9e)
INTERRUPT_HANDLER(0x9f)
INTERRUPT_HANDLER(0xa0)
INTERRUPT_HANDLER(0xa1)
INTERRUPT_HANDLER(0xa2)
INTERRUPT_HANDLER(0xa3)
INTERRUPT_HANDLER(0xa4)
INTERRUPT_HANDLER(0xa5)
INTERRUPT_HANDLER(0xa6)
INTERRUPT_HANDLER(0xa7)
INTERRUPT_HANDLER(0xa8)
INTERRUPT_HANDLER(0xa9)
INTERRUPT_HANDLER(0xaa)
INTERRUPT_HANDLER(0xab)
INTERRUPT_HANDLER(0xac)
INTERRUPT_HANDLER(0xad)
INTERRUPT_HANDLER(0xae)
INTERRUPT_HANDLER(0xaf)
INTERRUPT_HANDLER(0xb0)
INTERRUPT_HANDLER(0xb1)
INTERRUPT_HANDLER(0xb2)
INTERRUPT_HANDLER(0xb3)
INTERRUPT_HANDLER(0xb4)
INTERRUPT_HANDLER(0xb5)
INTERRUPT_HANDLER(0xb6)
INTERRUPT_HANDLER(0xb7)
INTERRUPT_HANDLER(0xb8)
INTERRUPT_HANDLER(0xb9)
INTERRUPT_HANDLER(0xba)
INTERRUPT_HANDLER(0xbb)
INTERRUPT_HANDLER(0xbc)
INTERRUPT_HANDLER(0xbd)
INTERRUPT_HANDLER(0xbe)
INTERRUPT_HANDLER(0xbf)
INTERRUPT_HANDLER(0xc0)
INTERRUPT_HANDLER(0xc1)
INTERRUPT_HANDLER(0xc2)
INTERRUPT_HANDLER(0xc3)
INTERRUPT_HANDLER(0xc4)
INTERRUPT_HANDLER(0xc5)
INTERRUPT_HANDLER(0xc6)
INTERRUPT_HANDLER(0xc7)
INTERRUPT_HANDLER(0xc8)
INTERRUPT_HANDLER(0xc9)
INTERRUPT_HANDLER(0xca)
INTERRUPT_HANDLER(0xcb)
INTERRUPT_HANDLER(0xcc)
INTERRUPT_HANDLER(0xcd)
INTERRUPT_HANDLER(0xce)
INTERRUPT_HANDLER(0xcf)
INTERRUPT_HANDLER(0xd0)
INTERRUPT_HANDLER(0xd1)
INTERRUPT_HANDLER(0xd2)
INTERRUPT_HANDLER(0xd3)
INTERRUPT_HANDLER(0xd4)
INTERRUPT_HANDLER(0xd5)
INTERRUPT_HANDLER(0xd6)
INTERRUPT_HANDLER(0xd7)
INTERRUPT_HANDLER(0xd8)
INTERRUPT_HANDLER(0xd9)
INTERRUPT_HANDLER(0xda)
INTERRUPT_HANDLER(0xdb)
INTERRUPT_HANDLER(0xdc)
INTERRUPT_HANDLER(0xdd)
INTERRUPT_HANDLER(0xde)
INTERRUPT_HANDLER(0xdf)
INTERRUPT_HANDLER(0xe0)
INTERRUPT_HANDLER(0xe1)
INTERRUPT_HANDLER(0xe2)
INTERRUPT_HANDLER(0xe3)
INTERRUPT_HANDLER(0xe4)
INTERRUPT_HANDLER(0xe5)
INTERRUPT_HANDLER(0xe6)
INTERRUPT_HANDLER(0xe7)
INTERRUPT_HANDLER(0xe8)
INTERRUPT_HANDLER(0xe9)
INTERRUPT_HANDLER(0xea)
INTERRUPT_HANDLER(0xeb)
INTERRUPT_HANDLER(0xec)
INTERRUPT_HANDLER(0xed)
INTERRUPT_HANDLER(0xee)
INTERRUPT_HANDLER(0xef)
INTERRUPT_HANDLER(0xf0)
INTERRUPT_HANDLER(0xf1)
INTERRUPT_HANDLER(0xf2)
INTERRUPT_HANDLER(0xf3)
INTERRUPT_HANDLER(0xf4)
INTERRUPT_HANDLER(0xf5)
INTERRUPT_HANDLER(0xf6)
INTERRUPT_HANDLER(0xf7)
INTERRUPT_HANDLER(0xf8)
INTERRUPT_HANDLER(0xf9)
INTERRUPT_HANDLER(0xfa)
INTERRUPT_HANDLER(0xfb)
INTERRUPT_HANDLER(0xfc)
INTERRUPT_HANDLER(0xfd)
INTERRUPT_HANDLER(0xfe)
INTERRUPT_HANDLER(0xff)

/* =============================================================================================================================================== */

void init_idt()
{
    trace("Initializing IDT");
    set_idt_entry(0x0, interrupt_handler_0x00, 1, 0);
    set_idt_entry(0x1, interrupt_handler_0x01, 1, 3);
    set_idt_entry(0x2, interrupt_handler_0x02, 2, 0);
    set_idt_entry(0x3, interrupt_handler_0x03, 1, 0);
    set_idt_entry(0x4, interrupt_handler_0x04, 1, 0);
    set_idt_entry(0x5, interrupt_handler_0x05, 1, 0);
    set_idt_entry(0x6, interrupt_handler_0x06, 1, 0);
    set_idt_entry(0x7, interrupt_handler_0x07, 1, 0);
    set_idt_entry(0x8, interrupt_handler_0x08, 3, 0);
    set_idt_entry(0x9, interrupt_handler_0x09, 1, 0);
    set_idt_entry(0xa, interrupt_handler_0x0a, 1, 0);
    set_idt_entry(0xb, interrupt_handler_0x0b, 1, 0);
    set_idt_entry(0xc, interrupt_handler_0x0c, 3, 0);
    set_idt_entry(0xd, interrupt_handler_0x0d, 3, 0);
    set_idt_entry(0xe, interrupt_handler_0x0e, 3, 0);
    set_idt_entry(0xf, interrupt_handler_0x0f, 1, 0);
    set_idt_entry(0x10, interrupt_handler_0x10, 1, 0);
    set_idt_entry(0x11, interrupt_handler_0x11, 1, 0);
    set_idt_entry(0x12, interrupt_handler_0x12, 1, 0);
    set_idt_entry(0x13, interrupt_handler_0x13, 1, 0);
    set_idt_entry(0x14, interrupt_handler_0x14, 1, 0);
    set_idt_entry(0x15, interrupt_handler_0x15, 1, 0);
    set_idt_entry(0x16, interrupt_handler_0x16, 1, 0);
    set_idt_entry(0x17, interrupt_handler_0x17, 1, 0);
    set_idt_entry(0x18, interrupt_handler_0x18, 1, 0);
    set_idt_entry(0x19, interrupt_handler_0x19, 1, 0);
    set_idt_entry(0x1a, interrupt_handler_0x1a, 1, 0);
    set_idt_entry(0x1b, interrupt_handler_0x1b, 1, 0);
    set_idt_entry(0x1c, interrupt_handler_0x1c, 1, 0);
    set_idt_entry(0x1d, interrupt_handler_0x1d, 1, 0);
    set_idt_entry(0x1e, interrupt_handler_0x1e, 1, 0);
    set_idt_entry(0x1f, interrupt_handler_0x1f, 1, 0);
    // IRQ
    set_idt_entry(0x20, interrupt_handler_0x20, 0, 0);
    set_idt_entry(0x21, interrupt_handler_0x21, 0, 0);
    set_idt_entry(0x22, interrupt_handler_0x22, 0, 0);
    set_idt_entry(0x23, interrupt_handler_0x23, 0, 0);
    set_idt_entry(0x24, interrupt_handler_0x24, 0, 0);
    set_idt_entry(0x25, interrupt_handler_0x25, 0, 0);
    set_idt_entry(0x26, interrupt_handler_0x26, 0, 0);
    set_idt_entry(0x27, interrupt_handler_0x27, 0, 0);
    set_idt_entry(0x28, interrupt_handler_0x28, 0, 0);
    set_idt_entry(0x29, interrupt_handler_0x29, 0, 0);
    set_idt_entry(0x2a, interrupt_handler_0x2a, 0, 0);
    set_idt_entry(0x2b, interrupt_handler_0x2b, 0, 0);
    set_idt_entry(0x2c, interrupt_handler_0x2c, 0, 0);
    set_idt_entry(0x2d, interrupt_handler_0x2d, 0, 0);
    set_idt_entry(0x2e, interrupt_handler_0x2e, 0, 0);
    set_idt_entry(0x2f, interrupt_handler_0x2f, 0, 0);

    set_idt_entry(0x30, interrupt_handler_0x30, 0, 0);
    set_idt_entry(0x31, interrupt_handler_0x31, 0, 0);
    set_idt_entry(0x32, interrupt_handler_0x32, 0, 0);
    set_idt_entry(0x33, interrupt_handler_0x33, 0, 0);
    set_idt_entry(0x34, interrupt_handler_0x34, 0, 0);
    set_idt_entry(0x35, interrupt_handler_0x35, 0, 0);
    set_idt_entry(0x36, interrupt_handler_0x36, 0, 0);
    set_idt_entry(0x37, interrupt_handler_0x37, 0, 0);
    set_idt_entry(0x38, interrupt_handler_0x38, 0, 0);
    set_idt_entry(0x39, interrupt_handler_0x39, 0, 0);
    set_idt_entry(0x3a, interrupt_handler_0x3a, 0, 0);
    set_idt_entry(0x3b, interrupt_handler_0x3b, 0, 0);
    set_idt_entry(0x3c, interrupt_handler_0x3c, 0, 0);
    set_idt_entry(0x3d, interrupt_handler_0x3d, 0, 0);
    set_idt_entry(0x3e, interrupt_handler_0x3e, 0, 0);
    set_idt_entry(0x3f, interrupt_handler_0x3f, 0, 0);
    set_idt_entry(0x40, interrupt_handler_0x40, 0, 0);
    set_idt_entry(0x41, interrupt_handler_0x41, 0, 0);
    set_idt_entry(0x42, interrupt_handler_0x42, 0, 0);
    set_idt_entry(0x43, interrupt_handler_0x43, 0, 0);
    set_idt_entry(0x44, interrupt_handler_0x44, 0, 0);
    set_idt_entry(0x45, interrupt_handler_0x45, 0, 0);
    set_idt_entry(0x46, interrupt_handler_0x46, 0, 0);
    set_idt_entry(0x47, interrupt_handler_0x47, 0, 0);
    set_idt_entry(0x48, interrupt_handler_0x48, 0, 0);
    set_idt_entry(0x49, interrupt_handler_0x49, 0, 0);
    set_idt_entry(0x4a, interrupt_handler_0x4a, 0, 0);
    set_idt_entry(0x4b, interrupt_handler_0x4b, 0, 0);
    set_idt_entry(0x4c, interrupt_handler_0x4c, 0, 0);
    set_idt_entry(0x4d, interrupt_handler_0x4d, 0, 0);
    set_idt_entry(0x4e, interrupt_handler_0x4e, 0, 0);
    set_idt_entry(0x4f, interrupt_handler_0x4f, 0, 0);
    set_idt_entry(0x50, interrupt_handler_0x50, 0, 0);
    set_idt_entry(0x51, interrupt_handler_0x51, 0, 0);
    set_idt_entry(0x52, interrupt_handler_0x52, 0, 0);
    set_idt_entry(0x53, interrupt_handler_0x53, 0, 0);
    set_idt_entry(0x54, interrupt_handler_0x54, 0, 0);
    set_idt_entry(0x55, interrupt_handler_0x55, 0, 0);
    set_idt_entry(0x56, interrupt_handler_0x56, 0, 0);
    set_idt_entry(0x57, interrupt_handler_0x57, 0, 0);
    set_idt_entry(0x58, interrupt_handler_0x58, 0, 0);
    set_idt_entry(0x59, interrupt_handler_0x59, 0, 0);
    set_idt_entry(0x5a, interrupt_handler_0x5a, 0, 0);
    set_idt_entry(0x5b, interrupt_handler_0x5b, 0, 0);
    set_idt_entry(0x5c, interrupt_handler_0x5c, 0, 0);
    set_idt_entry(0x5d, interrupt_handler_0x5d, 0, 0);
    set_idt_entry(0x5e, interrupt_handler_0x5e, 0, 0);
    set_idt_entry(0x5f, interrupt_handler_0x5f, 0, 0);
    set_idt_entry(0x60, interrupt_handler_0x60, 0, 0);
    set_idt_entry(0x61, interrupt_handler_0x61, 0, 0);
    set_idt_entry(0x62, interrupt_handler_0x62, 0, 0);
    set_idt_entry(0x63, interrupt_handler_0x63, 0, 0);
    set_idt_entry(0x64, interrupt_handler_0x64, 0, 0);
    set_idt_entry(0x65, interrupt_handler_0x65, 0, 0);
    set_idt_entry(0x66, interrupt_handler_0x66, 0, 0);
    set_idt_entry(0x67, interrupt_handler_0x67, 0, 0);
    set_idt_entry(0x68, interrupt_handler_0x68, 0, 0);
    set_idt_entry(0x69, interrupt_handler_0x69, 0, 0);
    set_idt_entry(0x6a, interrupt_handler_0x6a, 0, 0);
    set_idt_entry(0x6b, interrupt_handler_0x6b, 0, 0);
    set_idt_entry(0x6c, interrupt_handler_0x6c, 0, 0);
    set_idt_entry(0x6d, interrupt_handler_0x6d, 0, 0);
    set_idt_entry(0x6e, interrupt_handler_0x6e, 0, 0);
    set_idt_entry(0x6f, interrupt_handler_0x6f, 0, 0);
    set_idt_entry(0x70, interrupt_handler_0x70, 0, 0);
    set_idt_entry(0x71, interrupt_handler_0x71, 0, 0);
    set_idt_entry(0x72, interrupt_handler_0x72, 0, 0);
    set_idt_entry(0x73, interrupt_handler_0x73, 0, 0);
    set_idt_entry(0x74, interrupt_handler_0x74, 0, 0);
    set_idt_entry(0x75, interrupt_handler_0x75, 0, 0);
    set_idt_entry(0x76, interrupt_handler_0x76, 0, 0);
    set_idt_entry(0x77, interrupt_handler_0x77, 0, 0);
    set_idt_entry(0x78, interrupt_handler_0x78, 0, 0);
    set_idt_entry(0x79, interrupt_handler_0x79, 0, 0);
    set_idt_entry(0x7a, interrupt_handler_0x7a, 0, 0);
    set_idt_entry(0x7b, interrupt_handler_0x7b, 0, 0);
    set_idt_entry(0x7c, interrupt_handler_0x7c, 0, 0);
    set_idt_entry(0x7d, interrupt_handler_0x7d, 0, 0);
    set_idt_entry(0x7e, interrupt_handler_0x7e, 0, 0);
    set_idt_entry(0x7f, interrupt_handler_0x7f, 0, 0);
    set_idt_entry(0x80, interrupt_handler_0x80, 0, 0);
    set_idt_entry(0x81, interrupt_handler_0x81, 0, 0);
    set_idt_entry(0x82, interrupt_handler_0x82, 0, 0);
    set_idt_entry(0x83, interrupt_handler_0x83, 0, 0);
    set_idt_entry(0x84, interrupt_handler_0x84, 0, 0);
    set_idt_entry(0x85, interrupt_handler_0x85, 0, 0);
    set_idt_entry(0x86, interrupt_handler_0x86, 0, 0);
    set_idt_entry(0x87, interrupt_handler_0x87, 0, 0);
    set_idt_entry(0x88, interrupt_handler_0x88, 0, 0);
    set_idt_entry(0x89, interrupt_handler_0x89, 0, 0);
    set_idt_entry(0x8a, interrupt_handler_0x8a, 0, 0);
    set_idt_entry(0x8b, interrupt_handler_0x8b, 0, 0);
    set_idt_entry(0x8c, interrupt_handler_0x8c, 0, 0);
    set_idt_entry(0x8d, interrupt_handler_0x8d, 0, 0);
    set_idt_entry(0x8e, interrupt_handler_0x8e, 0, 0);
    set_idt_entry(0x8f, interrupt_handler_0x8f, 0, 0);
    set_idt_entry(0x90, interrupt_handler_0x90, 0, 0);
    set_idt_entry(0x91, interrupt_handler_0x91, 0, 0);
    set_idt_entry(0x92, interrupt_handler_0x92, 0, 0);
    set_idt_entry(0x93, interrupt_handler_0x93, 0, 0);
    set_idt_entry(0x94, interrupt_handler_0x94, 0, 0);
    set_idt_entry(0x95, interrupt_handler_0x95, 0, 0);
    set_idt_entry(0x96, interrupt_handler_0x96, 0, 0);
    set_idt_entry(0x97, interrupt_handler_0x97, 0, 0);
    set_idt_entry(0x98, interrupt_handler_0x98, 0, 0);
    set_idt_entry(0x99, interrupt_handler_0x99, 0, 0);
    set_idt_entry(0x9a, interrupt_handler_0x9a, 0, 0);
    set_idt_entry(0x9b, interrupt_handler_0x9b, 0, 0);
    set_idt_entry(0x9c, interrupt_handler_0x9c, 0, 0);
    set_idt_entry(0x9d, interrupt_handler_0x9d, 0, 0);
    set_idt_entry(0x9e, interrupt_handler_0x9e, 0, 0);
    set_idt_entry(0x9f, interrupt_handler_0x9f, 0, 0);
    set_idt_entry(0xa0, interrupt_handler_0xa0, 0, 0);
    set_idt_entry(0xa1, interrupt_handler_0xa1, 0, 0);
    set_idt_entry(0xa2, interrupt_handler_0xa2, 0, 0);
    set_idt_entry(0xa3, interrupt_handler_0xa3, 0, 0);
    set_idt_entry(0xa4, interrupt_handler_0xa4, 0, 0);
    set_idt_entry(0xa5, interrupt_handler_0xa5, 0, 0);
    set_idt_entry(0xa6, interrupt_handler_0xa6, 0, 0);
    set_idt_entry(0xa7, interrupt_handler_0xa7, 0, 0);
    set_idt_entry(0xa8, interrupt_handler_0xa8, 0, 0);
    set_idt_entry(0xa9, interrupt_handler_0xa9, 0, 0);
    set_idt_entry(0xaa, interrupt_handler_0xaa, 0, 0);
    set_idt_entry(0xab, interrupt_handler_0xab, 0, 0);
    set_idt_entry(0xac, interrupt_handler_0xac, 0, 0);
    set_idt_entry(0xad, interrupt_handler_0xad, 0, 0);
    set_idt_entry(0xae, interrupt_handler_0xae, 0, 0);
    set_idt_entry(0xaf, interrupt_handler_0xaf, 0, 0);
    set_idt_entry(0xb0, interrupt_handler_0xb0, 0, 0);
    set_idt_entry(0xb1, interrupt_handler_0xb1, 0, 0);
    set_idt_entry(0xb2, interrupt_handler_0xb2, 0, 0);
    set_idt_entry(0xb3, interrupt_handler_0xb3, 0, 0);
    set_idt_entry(0xb4, interrupt_handler_0xb4, 0, 0);
    set_idt_entry(0xb5, interrupt_handler_0xb5, 0, 0);
    set_idt_entry(0xb6, interrupt_handler_0xb6, 0, 0);
    set_idt_entry(0xb7, interrupt_handler_0xb7, 0, 0);
    set_idt_entry(0xb8, interrupt_handler_0xb8, 0, 0);
    set_idt_entry(0xb9, interrupt_handler_0xb9, 0, 0);
    set_idt_entry(0xba, interrupt_handler_0xba, 0, 0);
    set_idt_entry(0xbb, interrupt_handler_0xbb, 0, 0);
    set_idt_entry(0xbc, interrupt_handler_0xbc, 0, 0);
    set_idt_entry(0xbd, interrupt_handler_0xbd, 0, 0);
    set_idt_entry(0xbe, interrupt_handler_0xbe, 0, 0);
    set_idt_entry(0xbf, interrupt_handler_0xbf, 0, 0);
    set_idt_entry(0xc0, interrupt_handler_0xc0, 0, 0);
    set_idt_entry(0xc1, interrupt_handler_0xc1, 0, 0);
    set_idt_entry(0xc2, interrupt_handler_0xc2, 0, 0);
    set_idt_entry(0xc3, interrupt_handler_0xc3, 0, 0);
    set_idt_entry(0xc4, interrupt_handler_0xc4, 0, 0);
    set_idt_entry(0xc5, interrupt_handler_0xc5, 0, 0);
    set_idt_entry(0xc6, interrupt_handler_0xc6, 0, 0);
    set_idt_entry(0xc7, interrupt_handler_0xc7, 0, 0);
    set_idt_entry(0xc8, interrupt_handler_0xc8, 0, 0);
    set_idt_entry(0xc9, interrupt_handler_0xc9, 0, 0);
    set_idt_entry(0xca, interrupt_handler_0xca, 0, 0);
    set_idt_entry(0xcb, interrupt_handler_0xcb, 0, 0);
    set_idt_entry(0xcc, interrupt_handler_0xcc, 0, 0);
    set_idt_entry(0xcd, interrupt_handler_0xcd, 0, 0);
    set_idt_entry(0xce, interrupt_handler_0xce, 0, 0);
    set_idt_entry(0xcf, interrupt_handler_0xcf, 0, 0);
    set_idt_entry(0xd0, interrupt_handler_0xd0, 0, 0);
    set_idt_entry(0xd1, interrupt_handler_0xd1, 0, 0);
    set_idt_entry(0xd2, interrupt_handler_0xd2, 0, 0);
    set_idt_entry(0xd3, interrupt_handler_0xd3, 0, 0);
    set_idt_entry(0xd4, interrupt_handler_0xd4, 0, 0);
    set_idt_entry(0xd5, interrupt_handler_0xd5, 0, 0);
    set_idt_entry(0xd6, interrupt_handler_0xd6, 0, 0);
    set_idt_entry(0xd7, interrupt_handler_0xd7, 0, 0);
    set_idt_entry(0xd8, interrupt_handler_0xd8, 0, 0);
    set_idt_entry(0xd9, interrupt_handler_0xd9, 0, 0);
    set_idt_entry(0xda, interrupt_handler_0xda, 0, 0);
    set_idt_entry(0xdb, interrupt_handler_0xdb, 0, 0);
    set_idt_entry(0xdc, interrupt_handler_0xdc, 0, 0);
    set_idt_entry(0xdd, interrupt_handler_0xdd, 0, 0);
    set_idt_entry(0xde, interrupt_handler_0xde, 0, 0);
    set_idt_entry(0xdf, interrupt_handler_0xdf, 0, 0);
    set_idt_entry(0xe0, interrupt_handler_0xe0, 0, 0);
    set_idt_entry(0xe1, interrupt_handler_0xe1, 0, 0);
    set_idt_entry(0xe2, interrupt_handler_0xe2, 0, 0);
    set_idt_entry(0xe3, interrupt_handler_0xe3, 0, 0);
    set_idt_entry(0xe4, interrupt_handler_0xe4, 0, 0);
    set_idt_entry(0xe5, interrupt_handler_0xe5, 0, 0);
    set_idt_entry(0xe6, interrupt_handler_0xe6, 0, 0);
    set_idt_entry(0xe7, interrupt_handler_0xe7, 0, 0);
    set_idt_entry(0xe8, interrupt_handler_0xe8, 0, 0);
    set_idt_entry(0xe9, interrupt_handler_0xe9, 0, 0);
    set_idt_entry(0xea, interrupt_handler_0xea, 0, 0);
    set_idt_entry(0xeb, interrupt_handler_0xeb, 0, 0);
    set_idt_entry(0xec, interrupt_handler_0xec, 0, 0);
    set_idt_entry(0xed, interrupt_handler_0xed, 0, 0);
    set_idt_entry(0xee, interrupt_handler_0xee, 0, 0);
    set_idt_entry(0xef, interrupt_handler_0xef, 0, 0);
    set_idt_entry(0xf0, interrupt_handler_0xf0, 0, 0);
    set_idt_entry(0xf1, interrupt_handler_0xf1, 0, 0);
    set_idt_entry(0xf2, interrupt_handler_0xf2, 0, 0);
    set_idt_entry(0xf3, interrupt_handler_0xf3, 0, 0);
    set_idt_entry(0xf4, interrupt_handler_0xf4, 0, 0);
    set_idt_entry(0xf5, interrupt_handler_0xf5, 0, 0);
    set_idt_entry(0xf6, interrupt_handler_0xf6, 0, 0);
    set_idt_entry(0xf7, interrupt_handler_0xf7, 0, 0);
    set_idt_entry(0xf8, interrupt_handler_0xf8, 0, 0);
    set_idt_entry(0xf9, interrupt_handler_0xf9, 0, 0);
    set_idt_entry(0xfa, interrupt_handler_0xfa, 0, 0);
    set_idt_entry(0xfb, interrupt_handler_0xfb, 0, 0);
    set_idt_entry(0xfc, interrupt_handler_0xfc, 0, 0);
    set_idt_entry(0xfd, interrupt_handler_0xfd, 0, 0);
    set_idt_entry(0xfe, interrupt_handler_0xfe, 0, 0);
    set_idt_entry(0xff, interrupt_handler_0xff, 0, 0);
    lidt(idtr);
    PIC_remap(0x20, 0x28);
}
