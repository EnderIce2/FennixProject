#pragma once
#include <types.h>
#include <boot/gbp.h>
#include <heap.h>
#include "boot/limine.h"
#include "boot/stivale2.h"
#ifdef __cplusplus
#include "mem/xalloc/Xalloc.hpp"
#endif

extern uint8_t kernel_stack[STACK_SIZE];

extern GlobalBootParams earlyparams;
extern GlobalBootParams *bootparams;
extern SysFlags *sysflags;
extern bool ShowRecoveryScreen;
#ifdef __cplusplus
extern Xalloc::AllocatorV1 *UserAllocator;
#endif

EXTERNC void stivale2_initializator(struct stivale2_struct *bootloaderdata);

void KernelTask();
