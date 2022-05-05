#include <critical.hpp>
#include <asm.h>
#include <heap.h>

namespace Critical
{
    CriticalSectionData *Enter()
    {
        CriticalSectionData *Data = new CriticalSectionData;
        Data->EnableInterrupts = InterruptsEnabled();
        Data->CriticalLock.attempting_to_get = 0;
        Data->CriticalLock.count = 0;
        Data->CriticalLock.current_holder = 0;
        Data->CriticalLock.lock_dat = 0;
        Data->CriticalLock.lock_name = 0;
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
