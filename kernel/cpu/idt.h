#ifndef __FENNIX_KERNEL_IDT_H__
#define __FENNIX_KERNEL_IDT_H__

#include <types.h>
#include <interrupts.h>
#include <int.h>

#include "cpuid.h"

extern InterruptDescriptorTableDescriptor idtr;

EXTERNC void set_idt_entry(uint8_t idt, void (*handler)(), uint64_t ist, uint64_t ring);
EXTERNC void register_interrupt_handler(uint8_t vector, INTERRUPT_HANDLER handle);
EXTERNC void unregister_interrupt_handler(uint8_t vector);
EXTERNC void init_idt();

#endif // !__FENNIX_KERNEL_IDT_H__
