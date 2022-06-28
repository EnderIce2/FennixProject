#include <heap.h>
#include <string.h>
#include <sys.h>
#include <stdalign.h>
#include <asm.h>
#include <lock.h>
#include <symbols.hpp>
#define syskernel
#include <assert.h>
#ifdef DEBUG_MEM_ALLOCATION
#include <debug.h>
#endif
#include "../drivers/serial.h"
#include "liballoc/liballoc_1_1.h"
#include "liballoc/liballoc.h"
#include "defaultalloc/defaultalloc.hpp"
#define BUDDY_ALLOC_IMPLEMENTATION
#include "buddyalloc/buddy_alloc.h"
#include "xalloc/Xalloc.hpp"

using namespace PMM;

NEWLOCK(heap_lock);

static AllocationAlgorithm AlgorithmToUse = AllocationAlgorithm::NoAllocationAlgorithm;
static struct buddy *buddy = nullptr;
static Xalloc::AllocatorV1 *xalloc = nullptr;

void init_heap(AllocationAlgorithm Type)
{
    LOCK(heap_lock);
    switch (Type)
    {
    case AllocationAlgorithm::Default:
        InitHeap((void *)KERNEL_HEAP_BASE, 0x20);
        break;
    case AllocationAlgorithm::LibAlloc:
        break;
    case AllocationAlgorithm::LibAlloc11:
        break;
    case AllocationAlgorithm::BuddyAlloc:
    {
        alignas(max_align_t) unsigned char buddy_buf[buddy_sizeof(4096)];
        alignas(max_align_t) unsigned char data_buf[4096];
        {
            buddy = buddy_init(buddy_buf, data_buf, 4096);
            assert(buddy != NULL);
        }
        break;
    }
    case AllocationAlgorithm::XallocV1:
    {
        xalloc = new Xalloc::AllocatorV1((void *)KERNEL_HEAP_BASE, false, false);
        break;
    }
    default:
        panic("Unknown allocation algorithm!", true);
    }
    AlgorithmToUse = Type;
    UNLOCK(heap_lock);
}

void HeapFree(void *Address)
{
    LOCK(heap_lock);
    bool inten = InterruptsEnabled();
    CLI;
    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
        defPREFIX(free)(Address);
        break;
    case AllocationAlgorithm::LibAlloc:
        liballoc_free(Address);
        break;
    case AllocationAlgorithm::LibAlloc11:
        libPREFIX(free)(Address);
        break;
    case AllocationAlgorithm::BuddyAlloc:
        buddy_free(buddy, Address);
        break;
    case AllocationAlgorithm::XallocV1:
        xalloc->Free(Address);
        break;
    }
    UNLOCK(heap_lock);
    if (inten)
        STI;
}

void *HeapMalloc(size_t Size)
{
    LOCK(heap_lock);
    bool inten = InterruptsEnabled();
    CLI;
    void *ret;

    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
        ret = defPREFIX(malloc)(Size);
        break;
    case AllocationAlgorithm::LibAlloc:
        ret = liballoc_malloc(Size);
        break;
    case AllocationAlgorithm::LibAlloc11: // TODO: Make that the Liballoc 1.1 start at KERNEL_HEAP_BASE
        ret = libPREFIX(malloc)(Size);
        break;
    case AllocationAlgorithm::BuddyAlloc:
        ret = buddy_malloc(buddy, Size);
        break;
    case AllocationAlgorithm::XallocV1:
        ret = xalloc->Malloc(Size);
        break;
    }

    memset(ret, 0, Size);

    UNLOCK(heap_lock);
    if (inten)
        STI;
    return ret;
}

void *HeapCalloc(size_t n, size_t Size)
{
    LOCK(heap_lock);
    bool inten = InterruptsEnabled();
    CLI;
    void *ret;

    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
        ret = defPREFIX(calloc)(n, Size);
        break;
    case AllocationAlgorithm::LibAlloc:
        ret = liballoc_calloc(n, Size);
        break;
    case AllocationAlgorithm::LibAlloc11:
        ret = libPREFIX(calloc)(n, Size);
        break;
    case AllocationAlgorithm::BuddyAlloc:
        ret = buddy_calloc(buddy, n, Size);
        break;
    case AllocationAlgorithm::XallocV1:
        ret = xalloc->Calloc(n, Size);
        break;
    }

    memset(ret, 0, n * Size);

    UNLOCK(heap_lock);
    if (inten)
        STI;
    return ret;
}

void *HeapRealloc(void *Address, size_t Size)
{
    LOCK(heap_lock);
    bool inten = InterruptsEnabled();
    CLI;
    void *ret;

    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
        ret = defPREFIX(realloc)(Address, Size);
        break;
    case AllocationAlgorithm::LibAlloc:
        ret = liballoc_realloc(Address, Size);
        break;
    case AllocationAlgorithm::LibAlloc11:
        ret = libPREFIX(realloc)(Address, Size);
        break;
    case AllocationAlgorithm::BuddyAlloc:
        ret = buddy_realloc(buddy, Address, Size);
        break;
    case AllocationAlgorithm::XallocV1:
        ret = xalloc->Realloc(Address, Size);
        break;
    }

    memset(ret, 0, Size);

    UNLOCK(heap_lock);
    if (inten)
        STI;
    return ret;
}

void *RequestPage() { return KernelAllocator.RequestPage(); }

void FreePage(void *address) { KernelAllocator.FreePage(address); }

void *RequestPages(uint64_t pages) { return KernelAllocator.RequestPages(pages); }

void FreePages(void *address, uint64_t pages) { KernelAllocator.FreePages(address, pages); }

uint64_t GetFreeMemory() { return KernelAllocator.GetFreeRAM(); }

uint64_t GetUsedMemory() { return KernelAllocator.GetUsedRAM(); }

uint64_t GetReservedMemory() { return KernelAllocator.GetReservedRAM(); }

#ifdef DEBUG_MEM_ALLOCATION
void *dbg_malloc(size_t Size, string file, int line, string function)
{
    void *Request = HeapMalloc(Size);
    debug("ALLOCATION: Malloc( %lld )->%#llx called from %s:%d in %s", Size, Request, file, line, function);
    return Request;
}

void *dbg_calloc(size_t n, size_t Size, string file, int line, string function)
{
    void *Request = HeapCalloc(n, Size);
    debug("ALLOCATION: Calloc( %lld %lld )->%#llx called from %s:%d in %s", n, Size, Request, file, line, function);
    return Request;
}

void *dbg_realloc(void *Address, size_t Size, string file, int line, string function)
{
    void *Request = HeapRealloc(Address, Size);
    debug("ALLOCATION: Realloc( %llx %lld )->%#llx called from %s:%d in %s", Address, Size, Request, file, line, function);
    return Request;
}

void dbg_free(void *Address, string file, int line, string function)
{
    debug("ALLOCATION: Free( %#llx ) called from %s:%d in %s", Address, file, line, function);
    return HeapFree(Address);
}
#endif

void *operator new(size_t Size)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        if (Size == 0)
            Size = 1;
        warn("No allocator specified. Requesting %d page(s)...", Size / PAGE_SIZE + 1);
        return KernelAllocator.RequestPages(Size / PAGE_SIZE + 1);
    }

    if (Size == 0)
        warn("Trying to allocate 0 bytes!");

#ifdef DEBUG_MEM_ALLOCATION
    if (void *Pointer = dbg_malloc(Size, __FILE__, __LINE__, __FUNCTION__))
#else
    if (void *Pointer = HeapMalloc(Size))
#endif
        return Pointer;

    warn("new( %llx ) failed! [Request by %s] Trying again...", Size, SymTbl->GetSymbolFromAddress((uint64_t)__builtin_return_address(0)));
    for (size_t i = 0; i < 16; i++)
    {
        if (void *Pointer = HeapMalloc(Size))
            return Pointer;
        else
            warn("new( %llx ) failed! Retrying... (%d)", Size, i);
    }
    err("new( %llx ) failed!", Size);
    throw;
}

void *operator new[](size_t Size)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        if (Size == 0)
            Size = 1;
        warn("No allocator specified. Requesting %d page(s)...", Size / PAGE_SIZE + 1);
        return KernelAllocator.RequestPages(Size / PAGE_SIZE + 1);
    }

    if (Size == 0)
        warn("Trying to allocate 0 bytes!");

#ifdef DEBUG_MEM_ALLOCATION
    if (void *Pointer = dbg_malloc(Size, __FILE__, __LINE__, __FUNCTION__))
#else
    if (void *Pointer = HeapMalloc(Size))
#endif
        return Pointer;

    warn("new[]( %llx ) failed! [Request by %s] Trying again...", Size, SymTbl->GetSymbolFromAddress((uint64_t)__builtin_return_address(0)));
    for (size_t i = 0; i < 16; i++)
    {
        if (void *Pointer = HeapMalloc(Size))
            return Pointer;
        else
            warn("new[]( %llx ) failed! Retrying... (%d)", Size, i);
    }
    err("new[]( %llx ) failed!", Size);
    throw;
}

void operator delete(void *Pointer)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        warn("No allocator specified. Aborting...");
        return;
    }
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete[](void *Pointer)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        warn("No allocator specified. Aborting...");
        return;
    }
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete(void *Pointer, long unsigned int n)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        warn("No allocator specified. Aborting...");
        return;
    }
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete[](void *Pointer, long unsigned int n)
{
    if (AlgorithmToUse == AllocationAlgorithm::NoAllocationAlgorithm)
    {
        warn("No allocator specified. Aborting...");
        return;
    }
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}
