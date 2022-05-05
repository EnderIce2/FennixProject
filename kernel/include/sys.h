#pragma once
#include <interrupts.h>
#include <int.h>

EXTERNC void crash(string message);
EXTERNC void isrcrash(REGISTERS *regs);

#define panic(m) crash(m)
