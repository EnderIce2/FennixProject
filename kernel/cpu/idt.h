#ifndef __FENNIX_KERNEL_IDT_H__
#define __FENNIX_KERNEL_IDT_H__

#include <types.h>
#include <interrupts.h>

#include "cpuid.h"

// https://wiki.osdev.org/CPU_Registers_x86-64

typedef void (*INTERRUPT_HANDLER)(REGISTERS *);

// static struct CALLBACK *idt_callbacks = NULL;
void set_idt_entry(uint8_t idt, void (*handler)(), uint64_t ist, uint64_t ring);
void register_interrupt_handler(uint8_t vector, INTERRUPT_HANDLER handle);
void register_irq(uint8_t irq, uint8_t vector);
EXTERNC void init_idt();

#endif // !__FENNIX_KERNEL_IDT_H__