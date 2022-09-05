#ifndef __FENNIX_KERNEL_LOCK_H__
#define __FENNIX_KERNEL_LOCK_H__

#include <types.h>

typedef volatile struct
{
    uint32_t LockData;
    const char *CurrentHolder;
    const char *AttemptingToGet;
    const char *LockName;
    uint32_t Count;
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
    name.AttemptingToGet = __FUNCTION__; \
    name.LockName = #name;                \
    spinlock_lock(&name.LockData);         \
    name.CurrentHolder = __FUNCTION__;    \
    name.Count++;                          \
    debug_redirect_write("Locked '%s' +", #name)

#define DBG_UNLOCK(name)             \
    spinlock_unlock(&name.LockData); \
    name.Count--;                    \
    debug_redirect_write("Unlocked '%s' -", name.LockName)

/**
 * @brief Spinlock lock implementation called by assembly
 *
 */
#define LOCK(name)                         \
    name.AttemptingToGet = __FUNCTION__; \
    name.LockName = #name;                \
    spinlock_lock(&name.LockData);         \
    name.CurrentHolder = __FUNCTION__;    \
    name.Count++

/**
 * @brief Spinlock unlock implementation called by assembly
 *
 */
#define UNLOCK(name)                 \
    spinlock_unlock(&name.LockData); \
    name.Count--

END_EXTERNC

#ifdef __cplusplus

class SmartLock
{
private:
    LOCK *LockPointer = nullptr;
#ifdef DEBUG
    const char *NameTempBuffer = nullptr;
#endif

public:
    SmartLock(LOCK &lock, const char *name)
    {
        this->LockPointer = &lock;
        this->LockPointer->AttemptingToGet = name;
#ifdef DEBUG
        NameTempBuffer = new char[strlen(name) + 12];
        strcpy((char *)NameTempBuffer, "Smart Lock - ");
        strcat((char *)NameTempBuffer, name);
        this->LockPointer->LockName = NameTempBuffer;
#else
        this->LockPointer->LockName = name;
#endif
        spinlock_lock(&this->LockPointer->LockData);
        this->LockPointer->CurrentHolder = name;
        this->LockPointer->Count++;
        // debug_redirect_write("Locked '%s' +", this->LockPointer->LockName);
    }
    ~SmartLock()
    {
        spinlock_unlock(&this->LockPointer->LockData);
        this->LockPointer->Count--;
#ifdef DEBUG
        delete[] NameTempBuffer;
#endif
        // debug_redirect_write("Unlocked '%s' -", this->LockPointer->LockName);
    }
};

#define SL_CONCAT(x, y) x##y
#define SMART_LOCK(name) SmartLock SL_CONCAT(lock##_, __COUNTER__)(name, __FUNCTION__)

#endif

#endif // !__FENNIX_KERNEL_LOCK_H__
