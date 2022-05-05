#include "msg.hpp"

#include <internal_task.h>

namespace MessageManager
{
    void Send(uint64_t ThreadID, void *Buffer)
    {
        ThreadControlBlock *thread = SysGetThreadByTID(ThreadID);
        if (thread)
        {
            MessageQueue *queue = thread->Msg;
            if (queue)
            {
                struct MessageData data = {SysGetCurrentThread()->ThreadID, Buffer};
                queue->Messages.push_back(data);
            }
        }
    }

    MessageQueue *GetMessageQueue()
    {
        return SysGetCurrentThread()->Msg;
    }
}
