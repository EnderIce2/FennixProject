#include <critical.hpp>
#include <asm.h>
#include <heap.h>

namespace Critical
{
    CriticalSectionData *Enter()
    {
        CriticalSectionData *Data = new CriticalSectionData;
        Data->EnableInterrupts = InterruptsEnabled();
        Data->CriticalLock.AttemptingToGet = 0;
        Data->CriticalLock.Count = 0;
        Data->CriticalLock.CurrentHolder = 0;
        Data->CriticalLock.LockData = 0;
        Data->CriticalLock.LockName = 0;
        LOCK(Data->CriticalLock);
        CLI;
        return Data;
    }

    void Leave(CriticalSectionData *Data)
    {
        UNLOCK(Data->CriticalLock);
        if (Data->EnableInterrupts)
            STI;
        delete Data;
    }
}
