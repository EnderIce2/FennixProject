#pragma once
#include <types.h>

namespace Heap
{
    struct HeapSegHdr
    {
        uint64_t Length;
        HeapSegHdr *Next;
        HeapSegHdr *Last;
        bool IsFree;
        void CombineForward();
        void CombineBackward();
        HeapSegHdr *Split(uint64_t SplitLength);
    } __attribute__((aligned(16)));
}

#define defPREFIX(func) defk##func

void InitHeap(void *HeapAddress, uint64_t PageCount);

extern void defPREFIX(free)(void *Address);
extern void *defPREFIX(malloc)(uint64_t Size);
extern void *defPREFIX(calloc)(uint64_t n, uint64_t Size);
extern void *defPREFIX(realloc)(void *Address, uint64_t Size);
