#pragma once
#include <types.h>
#include <boot/gbp.h>
#include <heap.h>
#include "boot/stivale2.h"

extern uint8_t kernel_stack[STACK_SIZE];

extern GlobalBootParams earlyparams;
extern GlobalBootParams *bootparams;

EXTERNC void stivale2_initializator(struct stivale2_struct *bootloaderdata);

void KernelTask();
