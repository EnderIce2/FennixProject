#pragma once
#include <interrupts.h>
#include <int.h>

EXTERNC void crash(string message, bool clear);
void TriggerUserModeCrash(TrapFrame *regs);
EXTERNC void isrcrash(TrapFrame *regs);

#define panic(m, clear) crash(m, clear)
