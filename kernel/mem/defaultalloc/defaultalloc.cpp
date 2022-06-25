#include "defaultalloc.hpp"
#include <debug.h>
#include <heap.h>

using namespace Heap;

void *HeapStart;
void *HeapEnd;
HeapSegHdr *LastHdr;

void InitHeap(void *HeapAddress, uint64_t PageCount)
{
    trace("heap initialization %016p, %d", HeapAddress, PageCount);
    void *Position = HeapAddress;
    for (uint64_t i = 0; i < PageCount; i++)
    {
        void *Page = KernelAllocator.RequestPage();
        KernelPageTableManager.MapMemory(Position, Page, RW);
        trace("Preallocate Heap Memory (%#llx-%#llx [%#llx])...", Position, (uint64_t)Position + PAGE_SIZE, Page);
        Position = (void *)((uint64_t)Position + PAGE_SIZE);
    }
    uint64_t HeapLength = PageCount * PAGE_SIZE;
    HeapStart = HeapAddress;
    HeapEnd = (void *)((uint64_t)HeapStart + HeapLength);
    HeapSegHdr *StartSegment = (HeapSegHdr *)HeapAddress;
    StartSegment->Length = HeapLength - sizeof(HeapSegHdr);
    StartSegment->Next = nullptr;
    StartSegment->Last = nullptr;
    StartSegment->IsFree = true;
    LastHdr = StartSegment;
}

HeapSegHdr *HeapSegHdr::Split(uint64_t SplitLength)
{
    if (SplitLength < 0x10)
        return nullptr;
    int64_t SplitSegmentLength = Length - SplitLength - (sizeof(HeapSegHdr));
    if (SplitSegmentLength < 0x10)
        return nullptr;
    HeapSegHdr *NewSplitHdr = (HeapSegHdr *)((uint64_t)this + SplitLength + sizeof(HeapSegHdr));
    Next->Last = NewSplitHdr;
    NewSplitHdr->Next = Next;
    Next = NewSplitHdr;
    NewSplitHdr->Last = this;
    NewSplitHdr->Length = SplitSegmentLength;
    NewSplitHdr->IsFree = IsFree;
    Length = SplitLength;
    if (LastHdr == this)
        LastHdr = NewSplitHdr;
    return NewSplitHdr;
}

void ExpandHeap(uint64_t Length)
{
    if (Length % PAGE_SIZE)
    {
        Length -= Length % PAGE_SIZE;
        Length += PAGE_SIZE;
    }
    uint64_t PageCount = Length / PAGE_SIZE;
    HeapSegHdr *NewSegment = (HeapSegHdr *)HeapEnd;
    for (uint64_t i = 0; i < PageCount; i++)
    {
        void *Page = KernelAllocator.RequestPage();
        KernelPageTableManager.MapMemory(HeapEnd, Page, RW);
        trace("Expanding Heap Memory (%#llx-%#llx [%#llx])...", HeapEnd, (uint64_t)HeapEnd + PAGE_SIZE, Page);
        HeapEnd = (void *)((uint64_t)HeapEnd + PAGE_SIZE);
    }
    NewSegment->IsFree = true;
    NewSegment->Last = LastHdr;
    LastHdr->Next = NewSegment;
    LastHdr = NewSegment;
    NewSegment->Next = nullptr;
    NewSegment->Length = Length - sizeof(HeapSegHdr);
    NewSegment->CombineBackward();
}

void HeapSegHdr::CombineForward()
{
    if (Next == nullptr)
        return;
    if (Next->IsFree == false)
        return;
    if (Next == LastHdr)
        LastHdr = this;
    if (Next->Next != nullptr)
        Next->Next->Last = this;

    Length = Length + Next->Length + sizeof(HeapSegHdr);
    Next = Next->Next;
}

void HeapSegHdr::CombineBackward()
{
    if (Last != nullptr && Last->IsFree)
        Last->CombineForward();
}

void defPREFIX(free)(void *Address)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return;
    }
    HeapSegHdr *Segment = (HeapSegHdr *)Address - 1;
    Segment->IsFree = true;
    Segment->CombineForward();
    Segment->CombineBackward();
}

void *defPREFIX(malloc)(uint64_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }

    if (Size < 0x10)
    {
        // warn("Allocation size is too small, using 0x10 instead!");
        Size = 0x10;
    }

    // #ifdef DEBUG
    //     if (Size < 1024)
    //         debug("Allocating %dB", Size);
    //     else if (TO_KB(Size) < 1024)
    //         debug("Allocating %dKB", TO_KB(Size));
    //     else if (TO_MB(Size) < 1024)
    //         debug("Allocating %dMB", TO_MB(Size));
    //     else if (TO_GB(Size) < 1024)
    //         debug("Allocating %dGB", TO_GB(Size));
    // #endif

    if (Size % 0x10 > 0)
    { // it is not a multiple of 0x10
        Size -= (Size % 0x10);
        Size += 0x10;
    }
    if (Size == 0)
        return nullptr;

    HeapSegHdr *CurrentSegment = (HeapSegHdr *)HeapStart;
    while (true)
    {
        if (CurrentSegment->IsFree)
        {
            if (CurrentSegment->Length > Size)
            {
                CurrentSegment->Split(Size);
                CurrentSegment->IsFree = false;
                return (void *)((uint64_t)CurrentSegment + sizeof(HeapSegHdr));
            }
            if (CurrentSegment->Length == Size)
            {
                CurrentSegment->IsFree = false;
                return (void *)((uint64_t)CurrentSegment + sizeof(HeapSegHdr));
            }
        }
        if (CurrentSegment->Next == nullptr)
            break;
        CurrentSegment = CurrentSegment->Next;
    }
    ExpandHeap(Size);
    return defPREFIX(malloc)(Size);
}

void *defPREFIX(calloc)(uint64_t n, uint64_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }

    if (Size < 0x10)
    {
        // warn("Allocation size is too small, using 0x10 instead!");
        Size = 0x10;
    }

    void *Block = defPREFIX(malloc)(n * Size);
    if (Block)
        memset(Block, 0, n * Size);
    return Block;
}

void *defPREFIX(realloc)(void *Address, uint64_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }
    if (!Address && Size == 0)
    {
        defPREFIX(free)(Address);
        return nullptr;
    }
    else if (!Address)
    {
        return defPREFIX(calloc)(Size, sizeof(char));
    }

    if (Size < 0x10)
    {
        // warn("Allocation size is too small, using 0x10 instead!");
        Size = 0x10;
    }

    void *newAddress = defPREFIX(calloc)(Size, sizeof(char));
    memcpy(newAddress, Address, Size);
    return newAddress;
}
