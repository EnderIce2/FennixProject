#pragma once
#include <interrupts.h>

EXTERNC void crash(string message);
EXTERNC void isrcrash(REGISTERS *regs);

#define panic(m) crash(m)
