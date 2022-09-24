#include <ipc.hpp>
#include "ipc.hpp"

InterProcessCommunication::IPC *ipc = nullptr;

namespace InterProcessCommunication
{
    IPCHandle *IPC::RegisterHandle(IPCPort Port)
    {
        if (Port == 0)
            return nullptr;

        PCB *pcb = SysGetCurrentProcess();

        if (pcb->IPCHandles->Get((int)Port) != 0)
            return nullptr;

        IPCHandle *handle = new IPCHandle;
        handle->ID = -1;
        handle->Buffer = nullptr;
        handle->Length = 0;
        handle->Type = IPCOperationNone;
        handle->Listening = 0;
        handle->Error = IPCUnknown;
        pcb->IPCHandles->AddNode(Port, (uint64_t)handle);
        return handle;
    }

    IPCHandle *IPC::Wait(IPCPort port)
    {
        return nullptr;
    }

    IPCError IPC::Read(int PID, IPCPort port, void *buf, int size)
    {
        return IPCError{IPCUnknown};
    }

    IPCError IPC::Write(int PID, IPCPort port, void *buf, int size)
    {
        return IPCError{IPCUnknown};
    }

    void IPCServiceStub()
    {
        trace("IPC Service Started.");
        SysSetThreadPriority(1);
        // TODO: do something useful here, like, IPC event viewer or smth...
        while (1)
            ;
    }

    IPC::IPC()
    {
        IPCProc = SysCreateProcess("IPC Service", CBElevation::System);
        SysCreateThread(IPCProc, (uint64_t)IPCServiceStub, 0, 0);
    }

    IPC::~IPC()
    {
    }
}
