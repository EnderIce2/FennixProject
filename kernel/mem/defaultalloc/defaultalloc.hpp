#pragma once
#include <types.h>

namespace Heap
{
    struct HeapSegHdr
    {
        size_t Length;
        HeapSegHdr *Next;
        HeapSegHdr *Last;
        bool IsFree;
        void CombineForward();
        void CombineBackward();
        HeapSegHdr *Split(size_t SplitLength);
    } __attribute__((alligned(16)));
}

#define defPREFIX(func) defk##func

void InitHeap(void *HeapAddress, size_t PageCount);

extern void defPREFIX(free)(void *Address);
extern void *defPREFIX(malloc)(size_t Size);
extern void *defPREFIX(calloc)(size_t n, size_t Size);
extern void *defPREFIX(realloc)(void *Address, size_t Size);
