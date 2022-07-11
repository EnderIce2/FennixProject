#include <heap.h>
#include <sys.h>
#include <debug.h>
#include <lock.h>
#include <printf.h>
#include "../../kernel.h"

using namespace PMM;

NEWLOCK(pfa_lock);

uint64_t FreeMemory;
uint64_t ReservedMemory;
uint64_t UsedMemory;
bool Initialized = false;
PageFrameAllocator KernelAllocator;
uint64_t PageBitmapIndex = 0;

#ifdef DEBUG
static string MemTypeString[] =
    {
        "Error",           // 0
        "Free",            // 1
        "Reserved",        // 2
        "Unusable",        // 3
        "ACPIReclaimable", // 4
        "ACPIMemoryNVS",   // 5
        "Framebuffer",     // 6
        "Kernel",          // 7
        "Unknown"          // 8
};
#endif

void PageFrameAllocator::ReadMemoryMap()
{
    trace("reading memory map %d %d %#llx", earlyparams.mem.Entries, earlyparams.mem.Size, earlyparams.mem.memmap);
    if (Initialized)
    {
        panic("Memory management was already initialized! Please hard reboot!", true);
        return;
    }
    Initialized = true;
    void *LargestFreeMemorySegment = NULL;
    size_t LargestFreeMemorySegmentSize = 0;
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
        if (earlyparams.mem.memmap[i].Type == GBP_Free)
            if (earlyparams.mem.memmap[i].Size > LargestFreeMemorySegmentSize)
            {
                LargestFreeMemorySegment = (void *)earlyparams.mem.memmap[i].PhysicalAddress;
                LargestFreeMemorySegmentSize = earlyparams.mem.memmap[i].Size;
                debug("Largest free memory segment: %016p | size:%lld (%dKB)",
                      (void *)earlyparams.mem.memmap[i].PhysicalAddress,
                      earlyparams.mem.memmap[i].Size,
                      TO_KB(earlyparams.mem.memmap[i].Size));
            }
    uint64_t MemorySize = earlyparams.mem.Size;
    FreeMemory = MemorySize;
    // uint64_t BitmapSize = MemorySize / 4096 / 8 + 1;
    // InitBitmap(BitmapSize, LargestFreeMemorySegment);
    InitBitmap(ALIGN_UP((MemorySize / 0x1000) / 8, 0x1000), LargestFreeMemorySegment);
    ReservePages(0, MemorySize / 4096 + 1);
#ifdef DEBUG
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
        debug("%p %d\t\t%s", earlyparams.mem.memmap[i].PhysicalAddress, earlyparams.mem.memmap[i].Pages, MemTypeString[earlyparams.mem.memmap[i].Type]);
#endif
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
        if (earlyparams.mem.memmap[i].Type == GBP_Free)
            UnreservePages((void *)earlyparams.mem.memmap[i].PhysicalAddress, earlyparams.mem.memmap[i].Pages);
    ReservePages(0, 0x100); // Reserve between 0 and 0x100000
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
    this->Initalized = true;
}

void PageFrameAllocator::InitBitmap(size_t BitmapSize, void *BufferAddress)
{
    trace("initializing bitmap %016p, size: %d (%d KB)", BufferAddress, BitmapSize, TO_KB(BitmapSize));
    PageBitmap.Size = BitmapSize;
    PageBitmap.Buffer = (uint8_t *)BufferAddress;
    for (size_t i = 0; i < BitmapSize; i++)
        *(uint8_t *)(PageBitmap.Buffer + i) = 0;
}

void *PageFrameAllocator::RequestPage()
{
    LOCK(pfa_lock);
    for (; PageBitmapIndex < PageBitmap.Size * 8; PageBitmapIndex++)
    {
        if (PageBitmap[PageBitmapIndex] == true)
            continue;
        LockPage((void *)(PageBitmapIndex * 4096));
#ifdef DEBUG_MEM_ALLOCATION
        if (KernelPageTableManager.Initalized)
            debug("Requested page %#llx", (void *)(PageBitmapIndex * 4096));
#endif
        UNLOCK(pfa_lock);
        return (void *)(PageBitmapIndex * 4096);
    }
    // TODO: Page Frame Swap to file

    char buf[256] = {'\0'};
    sprintf_(buf, "Out of memory! (Free: %ldMB; Used: %ldMB; Reserved: %ldMB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory));
    panic(buf, true);
    UNLOCK(pfa_lock);
    return NULL;
}

void PageFrameAllocator::FreePage(void *Address)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr)
    {
        warn("Null address specified");
        // return;
    }
#endif
    LOCK(pfa_lock);
#ifdef DEBUG_MEM_ALLOCATION
    // debug("Freeing page %016p", Address); // spam
#endif
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false)
    {
#ifdef DEBUG_MEM_ALLOCATION
        warn("Trying to free already free page %016p", Address);
#endif
        UNLOCK(pfa_lock);
        return;
    }
    if (PageBitmap.Set(Index, false))
    {
        FreeMemory += 4096;
        UsedMemory -= 4096;
        if (PageBitmapIndex > Index)
            PageBitmapIndex = Index;
    }
    UNLOCK(pfa_lock);
}

void *PageFrameAllocator::RequestPages(uint64_t PageCount)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (PageCount == 0)
    {
        warn("Trying to request 0 pages");
        // PageCount = 1;
    }
#endif
    LOCK(pfa_lock);
    for (; PageBitmapIndex < PageBitmap.Size * 8; PageBitmapIndex++)
    {
        if (PageBitmap[PageBitmapIndex] == true)
            continue;

        for (uint64_t Index = PageBitmapIndex; Index < PageBitmap.Size * 8; Index++)
        {
            if (PageBitmap[Index] == true)
                continue;

            for (uint64_t i = 0; i < PageCount; i++)
                if (PageBitmap[Index + i] == true)
                    goto NextPage;

            LockPages((void *)(Index * 4096), PageCount);
#ifdef DEBUG_MEM_ALLOCATION
            debug("Requested pages %016p returning %#lx", PageCount, (void *)(Index * 4096));
#endif
            UNLOCK(pfa_lock);
            return (void *)(Index * 4096);

        NextPage:
            Index += PageCount;
            continue;
        }
    }
    err("out of memory (free: %dMB, used: %dMB, reserved: %dMB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory));
    UNLOCK(pfa_lock);
    return NULL;
}

void PageFrameAllocator::FreePages(void *Address, uint64_t PageCount)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr || PageCount == 0)
    {
        warn("Trying to free %s", Address ? "address" : "", PageCount ? "0 pages" : "");
        // return;
    }
#endif
    for (uint64_t t = 0; t < PageCount; t++)
        FreePage((void *)((uint64_t)Address + (t * 4096)));
}

void PageFrameAllocator::LockPage(void *Address)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr)
    {
        warn("Trying to lock null address");
        // return;
    }
#endif
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true)
        return;
    if (PageBitmap.Set(Index, true))
    {
        FreeMemory -= 4096;
        UsedMemory += 4096;
    }
}

void PageFrameAllocator::LockPages(void *Address, uint64_t PageCount)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr || PageCount == 0)
    {
        warn("Trying to lock %s", Address ? "address" : "", PageCount ? "0 pages" : "");
        // return;
    }
#endif
    for (uint64_t t = 0; t < PageCount; t++)
        LockPage((void *)((uint64_t)Address + (t * 4096)));
}

void PageFrameAllocator::UnreservePage(void *Address)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr)
    {
        warn("Trying to unreserve null address");
        // return;
    }
#endif
    uint64_t Index = (uint64_t)Address / 4096;
#ifdef DEBUG_MEM_ALLOCATION
    if (PageBitmap[Index] == false)
    {
        // err("Trying to unreserve already unreserved page %016p", Address); // spam
        // return;
    }
#endif
    if (PageBitmap.Set(Index, false))
    {
        FreeMemory += 4096;
        ReservedMemory -= 4096;
        if (PageBitmapIndex > Index)
            PageBitmapIndex = Index;
    }
}

void PageFrameAllocator::UnreservePages(void *Address, uint64_t PageCount)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr || PageCount == 0)
    {
        warn("Trying to unreserve %s", Address ? "address" : "", PageCount ? "0 pages" : "");
        // return;
    }
#endif
    for (uint64_t t = 0; t < PageCount; t++)
        UnreservePage((void *)((uint64_t)Address + (t * 4096)));
}

void PageFrameAllocator::ReservePage(void *Address)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr)
    {
        warn("Trying to reserve null address");
        // return;
    }
#endif
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true)
    {
#ifdef DEBUG_MEM_ALLOCATION
        warn("page already reserved");
#endif
        return;
    }
    if (PageBitmap.Set(Index, true))
    {
        FreeMemory -= 4096;
        ReservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void *Address, uint64_t PageCount)
{
#ifdef DEBUG_MEM_ALLOCATION
    if (Address == nullptr || PageCount == 0)
    {
        warn("Trying to reserve %s", Address ? "address" : "", PageCount ? "0 pages" : "");
        // return;
    }
#endif
    for (uint64_t t = 0; t < PageCount; t++)
        ReservePage((void *)((uint64_t)Address + (t * 4096)));
}

uint64_t PageFrameAllocator::GetFreeRAM() { return FreeMemory; }

uint64_t PageFrameAllocator::GetUsedRAM() { return UsedMemory; }

uint64_t PageFrameAllocator::GetReservedRAM() { return ReservedMemory; }
