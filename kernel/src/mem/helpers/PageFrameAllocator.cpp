#include <heap.h>
#include <sys.h>
#include <debug.h>
#include "../../kernel.h"

using namespace PMM;

uint64_t FreeMemory;
uint64_t ReservedMemory;
uint64_t UsedMemory;
bool Initialized = false;
PageFrameAllocator KernelAllocator;
uint64_t PageBitmapIndex = 0;

void PageFrameAllocator::ReadMemoryMap()
{
    trace("reading memory map %d %d %#x", earlyparams.mem.Entries, earlyparams.mem.Size, earlyparams.mem.memmap);
    if (Initialized)
    {
        panic("Memory management was already initialized! Please hard reboot!");
        return;
    }
    Initialized = true;
    void *LargestFreeMemorySegment = NULL;
    size_t LargestFreeMemorySegmentSize = 0;
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
    {
        if (earlyparams.mem.memmap[i].Type == GBP_Free)
        {
            if (earlyparams.mem.memmap[i].Size > LargestFreeMemorySegmentSize)
            {
                LargestFreeMemorySegment = (void *)earlyparams.mem.memmap[i].PhysicalAddress;
                LargestFreeMemorySegmentSize = earlyparams.mem.memmap[i].Size;
                debug("largest free mem seg: %016p | size:%lld (%dKB)",
                      (void *)earlyparams.mem.memmap[i].PhysicalAddress,
                      earlyparams.mem.memmap[i].Size,
                      TO_KB(earlyparams.mem.memmap[i].Size));
            }
        }
    }
    uint64_t MemorySize = earlyparams.mem.Size;
    FreeMemory = MemorySize;
    uint64_t BitmapSize = MemorySize / 4096 / 8 + 1;
    // uint64_t BitmapSize = ALIGN_UP((MemorySize / 0x1000) / 8, 0x1000);
    // InitBitmap(BitmapSize, LargestFreeMemorySegment);
    InitBitmap(BitmapSize, LargestFreeMemorySegment);
    ReservePages(0, MemorySize / 4096 + 1);
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
    {
        switch (earlyparams.mem.memmap[i].Type)
        {
        case GBP_Error:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Free:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Reserved:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Unusable:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_ACPIReclaimable:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_ACPIMemoryNVS:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Framebuffer:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Kernel:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        case GBP_Unknown:
            trace("Type: %d [%#llx-%#llx]",
                  earlyparams.mem.memmap[i].Type, earlyparams.mem.memmap[i].PhysicalAddress,
                  earlyparams.mem.memmap[i].PhysicalAddress + earlyparams.mem.memmap[i].Size);
            break;
        }

        if (earlyparams.mem.memmap[i].Type == GBP_Free)
        {
            UnreservePages((void *)earlyparams.mem.memmap[i].PhysicalAddress, earlyparams.mem.memmap[i].Pages);
        }
    }
    for (uint64_t i = 0; i < earlyparams.mem.Entries; i++)
    {
        if (earlyparams.mem.memmap[i].Type == GBP_Free)
        {
            UnreservePages((void *)earlyparams.mem.memmap[i].PhysicalAddress, earlyparams.mem.memmap[i].Pages);
        }
    }
    ReservePages(0, 0x100); // reserve between 0 and 0x100000
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
}

void PageFrameAllocator::InitBitmap(size_t BitmapSize, void *BufferAddress)
{
    trace("initializing bitmap %016p, size: %d (%d KB)", BufferAddress, BitmapSize, TO_KB(BitmapSize));
    PageBitmap.Size = BitmapSize;
    PageBitmap.Buffer = (uint8_t *)BufferAddress;
    for (size_t i = 0; i < BitmapSize; i++)
    {
        *(uint8_t *)(PageBitmap.Buffer + i) = 0;
    }
}

void *PageFrameAllocator::RequestPage()
{
#ifdef DEBUG
    static int once = 0;
    if (!once++)
        debug("Current allocation: free: %dMB, used: %dMB, reserved: %dMB (%dGB %dGB %dGB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory), TO_GB(FreeMemory), TO_GB(UsedMemory), TO_GB(ReservedMemory));
    if (once >= 10000)
        once = 0;
    else
        once++;
#endif
    for (; PageBitmapIndex < PageBitmap.Size * 8; PageBitmapIndex++)
    {
        if (PageBitmap[PageBitmapIndex] == true)
            continue;
        LockPage((void *)(PageBitmapIndex * 4096));
        return (void *)(PageBitmapIndex * 4096);
    }
    err("out of memory (free: %dMB, used: %dMB, reserved: %dMB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory));
    return NULL; // TODO: Page Frame Swap to file
}

void PageFrameAllocator::FreePage(void *Address)
{
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false)
        return;
    if (PageBitmap.Set(Index, false))
    {
        FreeMemory += 4096;
        UsedMemory -= 4096;
        if (PageBitmapIndex > Index)
            PageBitmapIndex = Index;
    }
}

void *PageFrameAllocator::RequestPages(uint64_t PageCount)
{
#ifdef DEBUG
    static int once = 0;
    if (!once++)
        debug("Current allocation: free: %dMB, used: %dMB, reserved: %dMB (%dGB %dGB %dGB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory), TO_GB(FreeMemory), TO_GB(UsedMemory), TO_GB(ReservedMemory));
    if (once >= 10000)
        once = 0;
    else
        once++;
#endif
    for (; PageBitmapIndex < PageBitmap.Size * 8; PageBitmapIndex++)
    {
        if (PageBitmap[PageBitmapIndex] == true)
            continue;
        LockPage((void *)(PageBitmapIndex * (4096 * PageCount)));
        return (void *)(PageBitmapIndex * (4096 * PageCount));
    }
    err("out of memory (free: %dMB, used: %dMB, reserved: %dMB)", TO_MB(FreeMemory), TO_MB(UsedMemory), TO_MB(ReservedMemory));
    return NULL; // TODO: Page Frame Swap to file
}

void PageFrameAllocator::FreePages(void *Address, uint64_t PageCount)
{
    for (uint64_t t = 0; t < PageCount; t++)
    {
        FreePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::LockPage(void *Address)
{
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
    for (uint64_t t = 0; t < PageCount; t++)
    {
        LockPage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void *Address)
{
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == false)
        return;
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
    for (uint64_t t = 0; t < PageCount; t++)
    {
        UnreservePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void *Address)
{
    uint64_t Index = (uint64_t)Address / 4096;
    if (PageBitmap[Index] == true)
        return;
    if (PageBitmap.Set(Index, true))
    {
        FreeMemory -= 4096;
        ReservedMemory += 4096;
    }
}

void PageFrameAllocator::ReservePages(void *Address, uint64_t PageCount)
{
    for (uint64_t t = 0; t < PageCount; t++)
    {
        ReservePage((void *)((uint64_t)Address + (t * 4096)));
    }
}

uint64_t PageFrameAllocator::GetFreeRAM()
{
    return FreeMemory;
}

uint64_t PageFrameAllocator::GetUsedRAM()
{
    return UsedMemory;
}

uint64_t PageFrameAllocator::GetReservedRAM()
{
    return ReservedMemory;
}
