#ifndef __FENNIX_KERNEL_LOCK_H__
#define __FENNIX_KERNEL_LOCK_H__

#include <types.h>

typedef volatile struct
{
    uint32_t lock_dat;
    const char *current_holder;
    const char *attempting_to_get;
    const char *lock_name;
    uint32_t count;
} LOCK;

#define NEWLOCK(name) static LOCK name = {0, 0, 0, 0, 0}

START_EXTERNC

unsigned long api_redirect_debug_write(int type, char *message, const char *file, int line, const char *function, ...);
#define debug_redirect_write(m, ...) api_redirect_debug_write(6, m, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

extern void spinlock_lock(volatile uint32_t *lock);
extern void spinlock_unlock(volatile uint32_t *lock);
extern uint64_t spinlock_check_and_lock(volatile uint32_t *lock);
extern uint64_t spinlock_with_timeout(volatile uint32_t *lock, uint64_t iterations);

/**
 * @brief Not implemented yet
 *
 */
#define TPL_LOCK(type) ((LOCK){.tpl = type}) // TODO: implement TPL

#ifdef __cplusplus
// TODO: In C++20 volatile is deprecated (and I should use atomic operations????). https://stackoverflow.com/questions/59223814/why-is-volatile-deprecated-in-c20
#pragma GCC diagnostic ignored "-Wvolatile"
#endif

#define DBG_LOCK(name)                     \
    name.attempting_to_get = __FUNCTION__; \
    name.lock_name = #name;                \
    spinlock_lock(&name.lock_dat);         \
    name.current_holder = __FUNCTION__;    \
    name.count++;                          \
    debug_redirect_write("locked '%s' +", #name)

#define DBG_UNLOCK(name)             \
    spinlock_unlock(&name.lock_dat); \
    name.count--;                    \
    debug_redirect_write("unlocked '%s' -", name.lock_name)

/**
 * @brief Spinlock lock implementation called by assembly
 *
 */
#define LOCK(name)                         \
    name.attempting_to_get = __FUNCTION__; \
    name.lock_name = #name;                \
    spinlock_lock(&name.lock_dat);         \
    name.current_holder = __FUNCTION__;    \
    name.count++

/**
 * @brief Spinlock unlock implementation called by assembly
 *
 */
#define UNLOCK(name)                 \
    spinlock_unlock(&name.lock_dat); \
    name.count--

END_EXTERNC

#endif // !__FENNIX_KERNEL_LOCK_H__