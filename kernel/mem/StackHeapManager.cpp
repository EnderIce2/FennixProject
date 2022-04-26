#include <heap.h>
#include <lock.h>
#include "../kernel.h"

using namespace PMM;
using namespace VMM;

NEWLOCK(stack_lock);

StackHeap::StackHeap *KernelStackAllocator = nullptr;

namespace StackHeap
{

    uint64_t HeapBase = KERNEL_STACK_HEAP_BASE;

    void *StackHeap::AllocateStack(bool User)
    {
        // TODO: fix stack allocator

        // if (HeapBase >= KERNEL_STACK_HEAP_END)
        // {
        // warn("Stack heap is full.");
        void *Address = KernelAllocator.RequestPage();
        if (User)
            KernelPageTableManager.MapMemory((void *)(Address), (void *)(Address), PTFlag::RW | PTFlag::US);
            trace("New stack allocated at %p", Address);
        return Address;
        // }
        // uint64_t ReturnHeap = HeapBase;
        // KernelPageTableManager.MapMemory((void *)(ReturnHeap), KernelAllocator.RequestPage(), RW);
        // memcpy((void *)ReturnHeap, 0, STACK_SIZE);
        // HeapBase += STACK_SIZE;
        // trace("New stack allocated at %p", (void *)ReturnHeap);
        // return (PageTable *)ReturnHeap;
    }

    void StackHeap::FreeStack(void *Address)
    {
        fixme("Not implemented");
        KernelAllocator.FreePage(Address);
    }

    StackHeap::StackHeap() { trace("Initialized."); }
    StackHeap::~StackHeap() { err("StackHeap destructor called"); }
}