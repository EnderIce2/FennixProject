#pragma once
#include <stdint.h>

struct MessageData
{
    bool Valid;
    uint64_t SourceTID;
    void *Buffer;
};

enum MsgAllowPrivilegeLevel
{
    PrivLvl_None = 0,
    PrivLvl_User = 1,
    PrivLvl_Kernel = 2,
    PrivLvl_Special = 3,
};

struct MsgAllowedPrivilegeLevel
{
    enum MsgAllowPrivilegeLevel PrivilegeLevel;
    void *SpecialPermission;
};

#define MAX_MESSAGES 0x100

struct MessageQueue
{
    struct MsgAllowedPrivilegeLevel AllowedPrivilegeLevel;
#ifdef __cplusplus
    MessageData Messages[MAX_MESSAGES] = {0};
#endif
};
