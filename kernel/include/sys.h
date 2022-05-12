#pragma once
#include <interrupts.h>
#include <int.h>

EXTERNC void crash(string message, bool clear);
EXTERNC void isrcrash(REGISTERS *regs);

#define panic(m, clear) crash(m, clear)
