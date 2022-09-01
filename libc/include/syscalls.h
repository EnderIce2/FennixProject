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
    _Shutdown,
    _Reboot,
    _FadeBootLogo,

    _GetFramebufferAddress,
    _GetFramebufferSize,
    _GetFramebufferWidth,
    _GetFramebufferHeight,
    _GetFramebufferPixelsPerScanLine,

    _RegisterInterruptHandler,
    _UnregisterInterruptHandler,

    _GetLastKeyboardScanCode,

    _FileOpen,
    _FileOpenWithParent,
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
    _FileFullPath,
    _FileChildrenSize,
    _FileGetChildren,

    _usleep,

    _DebugMessage,
};

enum FileStatus
{
    OK = 0,
    NOT_FOUND = 1,
    ACCESS_DENIED = 2,
    INVALID_NAME = 3,
    INVALID_PARAMETER = 4,
    INVALID_HANDLE = 5,
    INVALID_PATH = 6,
    INVALID_FILE = 7,
    INVALID_DEVICE = 8,
    NOT_EMPTY = 9,
    NOT_SUPPORTED = 10,
    INVALID_DRIVE = 11,
    VOLUME_IN_USE = 12,
    TIMEOUT = 13,
    NO_MORE_FILES = 14,
    END_OF_FILE = 15,
    FILE_EXISTS = 16,
    PIPE_BUSY = 17,
    PIPE_DISCONNECTED = 18,
    MORE_DATA = 19,
    NO_DATA = 20,
    PIPE_NOT_CONNECTED = 21,
    MORE_ENTRIES = 22,
    DIRECTORY_NOT_EMPTY = 23,
    NOT_A_DIRECTORY = 24,
    FILE_IS_A_DIRECTORY = 25,
    DIRECTORY_NOT_ROOT = 26,
    DIRECTORY_NOT_EMPTY_2 = 27,
    END_OF_MEDIA = 28,
    NO_MEDIA = 29,
    UNRECOGNIZED_MEDIA = 30,
    SECTOR_NOT_FOUND = 31
};

enum NodeFlags
{
    FS_ERROR = 0x0,
    FS_FILE = 0x01,
    FS_DIRECTORY = 0x02,
    FS_CHARDEVICE = 0x03,
    FS_BLOCKDEVICE = 0x04,
    FS_PIPE = 0x05,
    FS_SYMLINK = 0x06,
    FS_MOUNTPOINT = 0x08
};

typedef struct _File
{
    // Name and status of the file
    char Name[256];
    enum FileStatus Status;
    // If Status is not "OK", then the rest of the fields are not valid.
    uint64_t IndexNode;
    uint64_t Mask;
    uint64_t Mode;
    uint64_t Flags;
    uint64_t UserIdentifier, GroupIdentifier;
    uint64_t Address;
    uint64_t Length;
    // SYSTEM RESERVED
    void *Parent;
    void *Operator;
    // SYSTEM RESERVED
    void *Handle;
} File;

DEFINE_SYSCALL1(exit, _ProcessExit, int)
DEFINE_SYSCALL3(createProcess, _ProcessCreate, char *, uint64_t, uint64_t)
DEFINE_SYSCALL3(createThread, _ThreadCreate, uint64_t, uint64_t, uint64_t)
DEFINE_SYSCALL0(getCurrentProcess, _GetCurrentProcess)
DEFINE_SYSCALL0(getCurrentThread, _GetCurrentThread)
DEFINE_SYSCALL0(getCurrentProcessID, _GetCurrentProcessID)
DEFINE_SYSCALL0(getCurrentThreadID, _GetCurrentThreadID)
DEFINE_SYSCALL0(getScheduleMode, _GetScheduleMode)

DEFINE_SYSCALL4(createTask, _CreateTask, uint64_t, uint64_t, uint64_t, char *)
DEFINE_SYSCALL1(pushTask, _PushTask, uint64_t)
DEFINE_SYSCALL0(popTask, _PopTask)

DEFINE_SYSCALL0(requestPage, _RequestPage)
DEFINE_SYSCALL1(freePage, _FreePage, void *)
DEFINE_SYSCALL1(requestPages, _RequestPages, uint64_t)
DEFINE_SYSCALL2(freePages, _FreePages, void *, uint64_t)

DEFINE_SYSCALL0(systemInfo, _SystemInfo)
DEFINE_SYSCALL0(systemTime, _SystemTime)
DEFINE_SYSCALL1(systemTimeSet, _SystemTimeSet, uint64_t)
DEFINE_SYSCALL0(shutdown, _Shutdown)
DEFINE_SYSCALL0(reboot, _Reboot)
DEFINE_SYSCALL0(fadeBootLogo, _FadeBootLogo)

DEFINE_SYSCALL0(displayAddress, _GetFramebufferAddress)
DEFINE_SYSCALL0(displaySize, _GetFramebufferSize)
DEFINE_SYSCALL0(displayWidth, _GetFramebufferWidth)
DEFINE_SYSCALL0(displayHeight, _GetFramebufferHeight)
DEFINE_SYSCALL0(displayPixelsPerScanLine, _GetFramebufferPixelsPerScanLine)

DEFINE_SYSCALL2(registerinthnd, _RegisterInterruptHandler, int, void *)
DEFINE_SYSCALL1(unregisterinthnd, _UnregisterInterruptHandler, int)

DEFINE_SYSCALL0(getLastKeyboardScanCode, _GetLastKeyboardScanCode)

DEFINE_SYSCALL1(FileOpen, _FileOpen, char *)
DEFINE_SYSCALL2(FileOpenWithParent, _FileOpenWithParent, char *, File *)
DEFINE_SYSCALL1(FileClose, _FileClose, File *)
DEFINE_SYSCALL4(FileRead, _FileRead, File *, uint64_t, uint8_t *, uint64_t)
DEFINE_SYSCALL4(FileWrite, _FileWrite, File *, uint64_t, uint8_t *, uint64_t)
DEFINE_SYSCALL1(FileSize, _FileSize, File *)
DEFINE_SYSCALL1(FileFullPath, _FileFullPath, File *)
DEFINE_SYSCALL1(FileChildrenSize, _FileChildrenSize, File *)
DEFINE_SYSCALL2(FileGetChildren, _FileGetChildren, File *, uint64_t)

DEFINE_SYSCALL1(usleep, _usleep, unsigned long)

DEFINE_SYSCALL2(dbg, _DebugMessage, int, char *)
