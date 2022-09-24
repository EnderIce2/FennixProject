#pragma once
#include <internal_task.h>
#include <vector.hpp>

namespace InterProcessCommunication
{    class IPC
    {
    private:
        PCB *IPCProc;

    public:
        IPC();
        ~IPC();

        IPCHandle *RegisterHandle(IPCPort Port);
        IPCHandle *Wait(IPCPort port);
        IPCError Read(int pid, IPCPort port, void *buf, int size);
        IPCError Write(int pid, IPCPort port, void *buf, int size);
    };
}

extern InterProcessCommunication::IPC *ipc;
