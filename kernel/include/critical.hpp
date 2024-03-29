#include <stdint.h>
#include <lock.h>

#pragma GCC diagnostic ignored "-Wsubobject-linkage"

namespace Critical
{
    struct CriticalSectionData
    {
        bool EnableInterrupts;
        LOCK CriticalLock;
    };

    CriticalSectionData *Enter();
    void Leave(CriticalSectionData *Data);
}

#define EnterCriticalSection Critical::CriticalSectionData *CriticalSectionData = Critical::Enter()
#define LeaveCriticalSection Critical::Leave(CriticalSectionData)
