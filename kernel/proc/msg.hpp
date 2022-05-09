#pragma once
#include <msg.h>

namespace MessageManager
{
    void Remove(uint64_t Index);
    void SendByTID(uint64_t ThreadID, void *Buffer);
    void SendByName(char *Name, void *Buffer);
    MessageQueue *GetMessageQueue();
    void CreateListener(char *Name);
}
