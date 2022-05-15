#include <heap.h>
#include <string.h>
#include <sys.h>
#include <stdalign.h>
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

using namespace PMM;

static AllocationAlgorithm AlgorithmToUse;
static struct buddy *buddy;

void init_heap(AllocationAlgorithm Type)
{
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
    default:
        panic("Unknown allocation algorithm!", true);
    }
    AlgorithmToUse = Type;
}

void HeapFree(void *Address)
{
    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
        return defPREFIX(free)(Address);
        break;
    case AllocationAlgorithm::LibAlloc:
        return liballoc_free(Address);
        break;
    case AllocationAlgorithm::LibAlloc11:
        return libPREFIX(free)(Address);
        break;
    case AllocationAlgorithm::BuddyAlloc:
        return buddy_free(buddy, Address);
        break;
    default:
        return;
    }
}

void *HeapMalloc(size_t Size)
{
    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
    {
        void *Pointer = defPREFIX(malloc)(Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc:
    {
        void *Pointer = liballoc_malloc(Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc11:
    {
        // TODO: Make that the Liballoc 1.1 start at KERNEL_HEAP_BASE
        void *Pointer = libPREFIX(malloc)(Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::BuddyAlloc:
    {
        void *Pointer = buddy_malloc(buddy, Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    default:
        return 0;
    }
}

void *HeapCalloc(size_t n, size_t Size)
{
    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
    {
        void *Pointer = defPREFIX(calloc)(n, Size);
        memset(Pointer, 0, n * Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc:
    {
        void *Pointer = liballoc_calloc(n, Size);
        memset(Pointer, 0, n * Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc11:
    {
        void *Pointer = libPREFIX(calloc)(n, Size);
        memset(Pointer, 0, n * Size);
        return Pointer;
    }
    case AllocationAlgorithm::BuddyAlloc:
    {
        void *Pointer = buddy_calloc(buddy, n, Size);
        memset(Pointer, 0, n * Size);
        return Pointer;
    }
    default:
        return 0;
    }
}

void *HeapRealloc(void *Address, size_t Size)
{
    switch (AlgorithmToUse)
    {
    case AllocationAlgorithm::Default:
    {
        void *Pointer = defPREFIX(realloc)(Address, Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc:
    {
        void *Pointer = liballoc_realloc(Address, Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::LibAlloc11:
    {
        void *Pointer = libPREFIX(realloc)(Address, Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    case AllocationAlgorithm::BuddyAlloc:
    {
        void *Pointer = buddy_realloc(buddy, Address, Size);
        memset(Pointer, 0, Size);
        return Pointer;
    }
    default:
        return 0;
    }
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
    if (Size == 0)
        Size = 1;
#ifdef DEBUG_MEM_ALLOCATION
    if (void *Pointer = dbg_malloc(Size, __FILE__, __LINE__, __FUNCTION__))
    {
#else
    if (void *Pointer = HeapMalloc(Size))
    {
#endif
        memset(Pointer, 0, Size);
        return Pointer;
    }
    warn("new( %llx ) failed!", Size);
    throw;
}

void *operator new[](size_t Size)
{
    if (Size == 0)
        Size = 1;
#ifdef DEBUG_MEM_ALLOCATION
    if (void *Pointer = dbg_malloc(Size, __FILE__, __LINE__, __FUNCTION__))
    {
#else
    if (void *Pointer = HeapMalloc(Size))
    {
#endif
        memset(Pointer, 0, Size);
        return Pointer;
    }
    warn("new[]( %llx ) failed!", Size);
    throw;
}

void operator delete(void *Pointer)
{
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete[](void *Pointer)
{
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete(void *Pointer, long unsigned int n)
{
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}

void operator delete[](void *Pointer, long unsigned int n)
{
#ifdef DEBUG_MEM_ALLOCATION
    dbg_free(Pointer, __FILE__, __LINE__, __FUNCTION__);
#else
    HeapFree(Pointer);
#endif
}
