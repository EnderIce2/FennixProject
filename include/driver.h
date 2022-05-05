#pragma once
#include <stdint.h>
#include <stddef.h>

enum DriverType
{
    TYPE_UNKNOWN,
    TYPE_KEYBOARD,
    TYPE_MOUSE,
    TYPE_DISK,
    TYPE_NETWORK,
    TYPE_VIDEO,
    TYPE_AUDIO,
    TYPE_BLOCK,
    TYPE_FS,
    TYPE_SERIAL,
    TYPE_USB,
    TYPE_SCSI,
    TYPE_HID,
    TYPE_SENSOR
};

enum DriverBindType
{
    BIND_NULL,
    BIND_INTERRUPT,
    BIND_PROCESS,
    BIND_PCI
};

typedef struct _DriverBind
{
    DriverBindType Bind;
    union
    {
        struct
        {
            uint8_t Vector;
        } Interrupt;

        struct
        {
            uint32_t ProcessId;
        } Process;

        struct
        {
            uint16_t Class;
            uint16_t SubClass;
            uint16_t CrogIF;
        } PCI;
    };
} DriverBind;

enum KernelCallType
{
    KCALL_NULL,

    KCALL_REQ_PAGE,
    KCALL_FREE_PAGE,

    KCALL_KMALLOC,
    KCALL_KFREE,
    KCALL_KCALLOC,
    KCALL_KCREALLOC,

    KCALL_GET_BOOTPARAMS,

    KCALL_HOOK_INTERRUPT,
    KCALL_UNHOOK_INTERRUPT,
    KCALL_END_OF_INTERRUPT
};

struct DriverKernelMainData
{
    void *(*KFctCall)(KernelCallType, ...);
};

typedef uint64_t (*DriverFunction)(DriverKernelMainData *Data);
#define DRIVER_ENTRY static uint64_t _start(DriverKernelMainData *Data)

typedef struct _DriverDefinition
{
    char Name[256];
    DriverType Type;
    DriverBind Bind;
} DriverDefinition;

#define DRIVER __attribute__((section(".driverdata"), used)) static DriverDefinition DriverDataSectionStructure

#define DRIVER_SUCCESS 0xD500CE5
