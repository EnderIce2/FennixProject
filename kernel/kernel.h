#pragma once
#include <types.h>
#include <boot/gbp.h>
#include <heap.h>
#include "boot/limine.h"
#ifdef __cplusplus
#include "mem/xalloc/Xalloc.hpp"
#endif

typedef struct _SysFlags
{
    /**
     * @brief Debug mode "debug"
     */
    bool fennecsarethebest;
    /**
     * @brief The root filesystem "rootfs=hda0 or something like that"
     */
    char rootfs[512]; // TODO: more info about the disk
    /**
     * @brief No gpu driver
     */
    bool nogpu;
    /**
     * @brief No hpet timer
     */
    bool nohpet;
    /**
     * @brief Enter in emergency mode TODO:
     */
    bool emergency;
    /**
     * @brief Do not mount any filesystem or partition
     */
    bool nomount;
    /**
     * @brief Do not initialize the boot loading screen
     */
    bool noloadingscreen;
    /**
     * @brief Set tasking mode to monotasking
     *
     */
    bool monotasking;
} SysFlags;

extern uint8_t kernel_stack[STACK_SIZE];

extern GlobalBootParams earlyparams;
extern GlobalBootParams *bootparams;
extern SysFlags *sysflags;
extern bool ShowRecoveryScreen;
extern bool FadeScreenNow;
#ifdef __cplusplus
extern Xalloc::AllocatorV1 *UserAllocator;
#endif

EXTERNC void limine_initializator();

void KernelTask();
