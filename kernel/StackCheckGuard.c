/* ---- STACK GUARD ---- */

#ifndef STACK_CHK_GUARD_VALUE
#if __UINTPTR_MAX__ == __UINT32_MAX__
#define STACK_CHK_GUARD_VALUE 0x25F6CC8D
#else
#define STACK_CHK_GUARD_VALUE 0xBADFE2EC255A8572
#endif
#endif

__attribute__((weak)) __UINTPTR_TYPE__ __stack_chk_guard = 0;
__attribute__((weak)) __UINTPTR_TYPE__ __stack_chk_guard_init(void) { return STACK_CHK_GUARD_VALUE; }

static void __attribute__((constructor, no_stack_protector)) __construct_stk_chk_guard()
{
    if (__stack_chk_guard == 0)
        __stack_chk_guard = __stack_chk_guard_init();
}

__attribute__((weak, noreturn)) void __stack_chk_fail(void)
{
    while (1)
        __asm__ volatile("hlt");
}
