#include "msg.hpp"

#include <internal_task.h>
#include <vector.hpp>

struct listeners
{
    uint64_t id;
    char *name;
};

Vector<listeners> listen;

namespace MessageManager
{
    void AddQueue(MessageData Messages[], MessageData Data)
    {
        for (size_t i = 0; i < MAX_MESSAGES; i++)
            if (Messages[i].Valid = false)
            {
                Messages[i].Valid = true;
                Messages[i] = Data;
                break;
            }
    }

    void Remove(uint64_t Index)
    {
        ThreadControlBlock *thread = SysGetCurrentThread();
        MessageQueue *queue = thread->Msg;
        if (queue)
        {
            queue->Messages[Index].Valid = false;
            queue->Messages[Index].SourceTID = 0;
            queue->Messages[Index].Buffer = 0;
        }
    }

    void SendByTID(uint64_t ThreadID, void *Buffer)
    {
        ThreadControlBlock *thread = SysGetThreadByTID(ThreadID);
        if (thread)
        {
            MessageQueue *queue = thread->Msg;
            if (queue)
            {
                MessageData data = {true, SysGetCurrentThread()->ThreadID, Buffer};
                AddQueue(queue->Messages, data);
            }
        }
    }

    void SendByName(char *Name, void *Buffer)
    {
        foreach (auto var in listen)
        {
            if (strcmp(var.name, Name) == 0)
            {
                ThreadControlBlock *thread = SysGetThreadByTID(var.id);
                if (thread)
                {
                    MessageQueue *queue = thread->Msg;
                    if (queue)
                    {
                        MessageData data = {true, SysGetCurrentThread()->ThreadID, Buffer};
                        AddQueue(queue->Messages, data);
                    }
                }
            }
        }
    }

    MessageQueue *GetMessageQueue()
    {
        return SysGetCurrentThread()->Msg;
    }

    void CreateListener(char *Name)
    {
        listen.push_back({SysGetCurrentThread()->ThreadID, Name});
    }
}