#ifndef __FENNIX_KERNEL_INTERRUPTS_H__
#define __FENNIX_KERNEL_INTERRUPTS_H__

#include <types.h>
#include <interrupts.h>
#include <cputables.h>

EXTERNC void EndOfInterrupt(int interrupt);

typedef void (*INTERRUPT_HANDLER)(TrapFrame *);
typedef unsigned int InterruptVector;

InterruptVector RegisterInterrupt(INTERRUPT_HANDLER Handler);
#ifdef __cplusplus
/**
 * @brief Register interrupt to an interrupt handler.
 * 
 * @param Handler Handler
 * @param Vector Vector to register
 * @param Override Override if already registered (Some interrupts are unable to be registered even if this is set to true)
 * @param RedirectIRQ Enable APIC vector redirecting
 * @param RedirectVector APIC vector (Leaving 0xDEADBEEF it will be calculated automatically (Vector - IRQ0))
 * @return true if successful, false otherwise
 */
bool RegisterInterrupt(INTERRUPT_HANDLER Handler, InterruptVector Vector, bool Override, bool RedirectIRQ = false, InterruptVector RedirectVector = 0xDEADBEEF);
#else
bool CRegisterInterrupt(INTERRUPT_HANDLER Handler, InterruptVector Vector, bool Override);
#endif
bool UnregisterInterrupt(InterruptVector vector);

#endif // !__FENNIX_KERNEL_INTERRUPTS_H__
