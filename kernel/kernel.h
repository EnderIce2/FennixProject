#pragma once
#include <types.h>
#include <boot/gbp.h>
#include <heap.h>
#include "boot/limine.h"
#include "boot/stivale.h"
#include "boot/stivale2.h"

extern uint8_t kernel_stack[STACK_SIZE];

extern GlobalBootParams earlyparams;
extern GlobalBootParams *bootparams;
extern SysFlags *sysflags;

EXTERNC void stivale_initializator(struct stivale_struct *bootloaderdata);
EXTERNC void stivale2_initializator(struct stivale2_struct *bootloaderdata);
