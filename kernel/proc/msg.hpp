#pragma once
#include <msg.h>

namespace MessageManager
{
    void Send(uint64_t ThreadID, void *Buffer);
    MessageQueue *GetMessageQueue();
}
