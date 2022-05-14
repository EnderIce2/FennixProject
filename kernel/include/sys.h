#pragma once
#include <interrupts.h>
#include <int.h>

EXTERNC void crash(string message, bool clear);
void TriggerUserModeCrash(REGISTERS *regs);
EXTERNC void isrcrash(REGISTERS *regs);

#define panic(m, clear) crash(m, clear)
