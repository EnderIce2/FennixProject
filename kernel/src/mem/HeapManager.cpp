#include <heap.h>
#include <string.h>
#include "../drivers/serial.h"

using namespace Heap;
using namespace PMM;

void *HeapStart;
void *HeapEnd;
HeapSegHdr *LastHdr;

void init_heap(void *HeapAddress, size_t PageCount)
{
    trace("heap initialization %016p, %d", HeapAddress, PageCount);
    void *Position = HeapAddress;
    for (size_t i = 0; i < PageCount; i++)
    {
        KernelPageTableManager.MapMemory(Position, KernelAllocator.RequestPage(), RW | PCD);
        Position = (void *)((size_t)Position + PAGE_SIZE);
    }
    size_t HeapLength = PageCount * PAGE_SIZE;
    HeapStart = HeapAddress;
    HeapEnd = (void *)((size_t)HeapStart + HeapLength);
    HeapSegHdr *StartSegment = (HeapSegHdr *)HeapAddress;
    StartSegment->Length = HeapLength - sizeof(HeapSegHdr);
    StartSegment->Next = nullptr;
    StartSegment->Last = nullptr;
    StartSegment->IsFree = true;
    LastHdr = StartSegment;
}

void kfree(void *Address)
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

void *kmalloc(size_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }
    if (Size % 0x10 > 0)
    { // it is not a multiple of 0x10
        Size -= (Size % 0x10);
        Size += 0x10;
    }
    if (Size == 0)
    {
        return nullptr;
    }
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
    return kmalloc(Size);
}

void *kcalloc(size_t n, size_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }
    void *Block = kmalloc(n * Size);
    if (Block)
        memset(Block, 0, n * Size);
    return Block;
}

void *krealloc(void *Address, size_t Size)
{
    if (HeapStart == nullptr)
    {
        err("Memory allocation not initialized yet!");
        return 0;
    }
    if (!Address && Size == 0)
    {
        kfree(Address);
        return nullptr;
    }
    else if (!Address)
    {
        return kcalloc(Size, sizeof(char));
    }
    void *newAddress = kcalloc(Size, sizeof(char));
    memcpy(newAddress, Address, Size);
    return newAddress;
}

HeapSegHdr *HeapSegHdr::Split(size_t SplitLength)
{
    if (SplitLength < 0x10)
        return nullptr;
    int64_t SplitSegmentLength = Length - SplitLength - (sizeof(HeapSegHdr));
    if (SplitSegmentLength < 0x10)
        return nullptr;
    HeapSegHdr *NewSplitHdr = (HeapSegHdr *)((size_t)this + SplitLength + sizeof(HeapSegHdr));
    Next->Last = NewSplitHdr;                 // Set the Next segment's Last segment to our new segment
    NewSplitHdr->Next = Next;                 // Set the new segment's Next segment to out original Next segment
    Next = NewSplitHdr;                       // Set our new segment to the new segment
    NewSplitHdr->Last = this;                 // Set our new segment's Last segment to the current segment
    NewSplitHdr->Length = SplitSegmentLength; // Set the new header's Length to the calculated value
    NewSplitHdr->IsFree = IsFree;             // make sure the new segment's IsFree is the same as the original
    Length = SplitLength;                     // set the Length of the original segment to its new Length
    if (LastHdr == this)
        LastHdr = NewSplitHdr;
    return NewSplitHdr;
}

void ExpandHeap(size_t Length)
{
    if (Length % PAGE_SIZE)
    {
        Length -= Length % PAGE_SIZE;
        Length += PAGE_SIZE;
    }
    size_t PageCount = Length / PAGE_SIZE;
    HeapSegHdr *NewSegment = (HeapSegHdr *)HeapEnd;
    for (size_t i = 0; i < PageCount; i++)
    {
        KernelPageTableManager.MapMemory(HeapEnd, KernelAllocator.RequestPage(), RW | PCD);
        HeapEnd = (void *)((size_t)HeapEnd + PAGE_SIZE);
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
    {
        Next->Next->Last = this;
    }
    Length = Length + Next->Length + sizeof(HeapSegHdr);
    Next = Next->Next;
}

void HeapSegHdr::CombineBackward()
{
    if (Last != nullptr && Last->IsFree)
        Last->CombineForward();
}

void *RequestPage()
{
    void *Request = KernelAllocator.RequestPage();
    return Request;
}

void FreePage(void *address)
{
    KernelAllocator.FreePage(address);
}

void *RequestPages(uint64_t pages)
{
    void *Request = KernelAllocator.RequestPage();
    return Request;
}

void FreePages(void *address, uint64_t pages)
{
    KernelAllocator.FreePages(address, pages);
}

uint64_t GetFreeMemory()
{
    return KernelAllocator.GetFreeRAM();
}

uint64_t GetUsedMemory()
{
    return KernelAllocator.GetUsedRAM();
}

uint64_t GetReservedMemory()
{
    return KernelAllocator.GetReservedRAM();
}

void *operator new(size_t Size)
{
    if (Size == 0)
        ++Size;
    if (void *Pointer = kmalloc(Size))
        return Pointer;
    throw;
}

void *operator new[](size_t Size)
{
    if (Size == 0)
        ++Size;
    if (void *Pointer = kmalloc(Size))
        return Pointer;
    throw;
}

void operator delete(void *Pointer)
{
    kfree(Pointer);
}

void operator delete[](void *Pointer)
{
    kfree(Pointer);
}

void operator delete(void *Pointer, long unsigned int n)
{
    kfree(Pointer);
    (void)(n);
}

void operator delete[](void *Pointer, long unsigned int n)
{
    kfree(Pointer);
    (void)(n);
}
