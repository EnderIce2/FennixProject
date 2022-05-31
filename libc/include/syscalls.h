#pragma once
#include <stdint.h>

#define DEFINE_SYSCALL0(function, n)          \
    static inline long syscall_##function()   \
    {                                         \
        long a = n;                           \
        __asm__ __volatile__("pushq %%r11\n"  \
                             "pushq %%rcx\n"  \
                             "syscall\n"      \
                             "popq %%rcx\n"   \
                             "popq %%r11\n"   \
                             : "=a"(a)        \
                             : "a"((long)a)); \
        return a;                             \
    }

#define DEFINE_SYSCALL1(function, n, a1)                        \
    static inline long syscall_##function(a1 arg1)              \
    {                                                           \
        long result = n;                                        \
        __asm__ __volatile__("pushq %%r11\n"                    \
                             "pushq %%rcx\n"                    \
                             "syscall\n"                        \
                             "popq %%rcx\n"                     \
                             "popq %%r11\n"                     \
                             : "=a"(result)                     \
                             : "a"(result), "b"((long)(arg1))); \
        return result;                                          \
    }

#define DEFINE_SYSCALL2(function, n, a1, a2)                                       \
    static inline long syscall_##function(a1 arg1, a2 arg2)                        \
    {                                                                              \
        long result = n;                                                           \
        __asm__ __volatile__("pushq %%r11\n"                                       \
                             "pushq %%rcx\n"                                       \
                             "syscall\n"                                           \
                             "popq %%rcx\n"                                        \
                             "popq %%r11\n"                                        \
                             : "=a"(result)                                        \
                             : "a"(result), "b"((long)(arg1)), "d"((long)(arg2))); \
        return result;                                                             \
    }

#define DEFINE_SYSCALL3(function, n, a1, a2, a3)                                                      \
    static inline long syscall_##function(a1 arg1, a2 arg2, a3 arg3)                                  \
    {                                                                                                 \
        long result = n;                                                                              \
        __asm__ __volatile__("pushq %%r11\n"                                                          \
                             "pushq %%rcx\n"                                                          \
                             "syscall\n"                                                              \
                             "popq %%rcx\n"                                                           \
                             "popq %%r11\n"                                                           \
                             : "=a"(result)                                                           \
                             : "a"(result), "b"((long)(arg1)), "d"((long)(arg2)), "S"((long)(arg3))); \
        return result;                                                                                \
    }

#define DEFINE_SYSCALL4(function, n, a1, a2, a3, a4)                                                                     \
    static inline long syscall_##function(a1 arg1, a2 arg2, a3 arg3, a4 arg4)                                            \
    {                                                                                                                    \
        long result = n;                                                                                                 \
        __asm__ __volatile__("pushq %%r11\n"                                                                             \
                             "pushq %%rcx\n"                                                                             \
                             "syscall\n"                                                                                 \
                             "popq %%rcx\n"                                                                              \
                             "popq %%r11\n"                                                                              \
                             : "=a"(result)                                                                              \
                             : "a"(result), "b"((long)(arg1)), "d"((long)(arg2)), "S"((long)(arg3)), "D"((long)(arg4))); \
        return result;                                                                                                   \
    }

// #define DEFINE_SYSCALL5(function, n, a1, a2, a3, a4, a5)                                                                                    \
//     static inline long syscall_##function(a1 arg1, a2 arg2, a3 arg3, a4 arg4, a5 arg5)                                                                    \
//     {                                                                                                                                       \
//         long result = n;                                                                                                                    \
//         __asm__ __volatile__("pushq %%r11\n"                                                                                                \
//                              "pushq %%rcx\n"                                                                                                \
//                              "syscall\n"                                                                                                    \
//                              "popq %%rcx\n"                                                                                                 \
//                              "popq %%r11\n"                                                                                                 \
//                              : "=a"(result)                                                                                                 \
//                              : "a"(result), "b"((long)(arg1)), "c"((long)(arg2)), "d"((long)(arg3)), "S"((long)(arg4)), "D"((long)(arg4))); \
//         return result;                                                                                                                      \
//     }

#define deadcall -0xdeadca11
#define failedcall -0xfa11edca11
#define deniedcall -0xde61edca11

enum SystemCalls
{
    _NullCall = 0,
    _ProcessExit,
    _ProcessCreate,
    _ThreadCreate,
    _GetCurrentProcess,
    _GetCurrentThread,
    _GetCurrentProcessID,
    _GetCurrentThreadID,

    _GetScheduleMode,

    _CreateTask,
    _PushTask,
    _PopTask,

    _RequestPage,
    _FreePage,
    _RequestPages,
    _FreePages,

    _SystemInfo,
    _SystemTime,
    _SystemTimeSet,

    _GetFramebufferAddress,
    _GetFramebufferSize,
    _GetFramebufferWidth,
    _GetFramebufferHeight,
    _GetFramebufferPixelsPerScanLine,

    _RegisterInterruptHandler,
    _UnregisterInterruptHandler,

    _GetLastKeyboardScanCode,

    _FileOpen,
    _FileClose,
    _FileRead,
    _FileWrite,
    _FileSeek,
    _FileSize,
    _FileFlush,
    _FileDelete,
    _FileRename,
    _FileExists,
    _FileCreate,

    _usleep,

    _DebugMessage,
};

DEFINE_SYSCALL1(exit, _ProcessExit, int)
DEFINE_SYSCALL3(createProcess, _ProcessCreate, char *, uint64_t, uint64_t)
DEFINE_SYSCALL3(createThread, _ThreadCreate, uint64_t, uint64_t, uint64_t)
DEFINE_SYSCALL0(getCurrentProcess, _GetCurrentProcess)
DEFINE_SYSCALL0(getCurrentThread, _GetCurrentThread)
DEFINE_SYSCALL0(getCurrentProcessID, _GetCurrentProcessID)
DEFINE_SYSCALL0(getCurrentThreadID, _GetCurrentThreadID)
DEFINE_SYSCALL0(getScheduleMode, _GetScheduleMode)

DEFINE_SYSCALL4(createTask, _CreateTask, uint64_t, uint64_t, uint64_t, char *)
DEFINE_SYSCALL0(pushTask, _PushTask)
DEFINE_SYSCALL0(popTask, _PopTask)

DEFINE_SYSCALL0(requestPage, _RequestPage)
DEFINE_SYSCALL1(freePage, _FreePage, void *)
DEFINE_SYSCALL1(requestPages, _RequestPages, uint64_t)
DEFINE_SYSCALL2(freePages, _FreePages, void *, uint64_t)

DEFINE_SYSCALL0(systemInfo, _SystemInfo)
DEFINE_SYSCALL0(systemTime, _SystemTime)
DEFINE_SYSCALL1(systemTimeSet, _SystemTimeSet, uint64_t)

DEFINE_SYSCALL0(displayAddress, _GetFramebufferAddress)
DEFINE_SYSCALL0(displaySize, _GetFramebufferSize)
DEFINE_SYSCALL0(displayWidth, _GetFramebufferWidth)
DEFINE_SYSCALL0(displayHeight, _GetFramebufferHeight)
DEFINE_SYSCALL0(displayPixelsPerScanLine, _GetFramebufferPixelsPerScanLine)

DEFINE_SYSCALL2(registerinthnd, _RegisterInterruptHandler, int, void *)
DEFINE_SYSCALL1(unregisterinthnd, _UnregisterInterruptHandler, int)

DEFINE_SYSCALL0(getLastKeyboardScanCode, _GetLastKeyboardScanCode)

DEFINE_SYSCALL1(FileOpen, _FileOpen, char *)
DEFINE_SYSCALL1(FileClose, _FileClose, void *)
DEFINE_SYSCALL4(FileRead, _FileRead, void *, uint64_t, void *, uint64_t)
DEFINE_SYSCALL4(FileWrite, _FileWrite, void *, uint64_t, void *, uint64_t)
DEFINE_SYSCALL1(FileSize, _FileSize, void *)

DEFINE_SYSCALL1(usleep, _usleep, unsigned long)

DEFINE_SYSCALL2(dbg, _DebugMessage, int, char *)
