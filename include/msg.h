#pragma once
#ifdef __cplusplus
#include <vector.hpp>
#endif
#include <stdint.h>

struct MessageData
{
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

struct MessageQueue
{
    struct MsgAllowedPrivilegeLevel AllowedPrivilegeLevel;
#ifdef __cplusplus
    Vector<struct MessageData> Messages;
#else
    void *Messages;
#endif
};
