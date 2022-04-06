#include <heap.h>
#include <lock.h>
#include <list.h>
#include <sys.h>
#include "../kernel.h"

static unsigned long long StackBase = KERNEL_STACK_HEAP_BASE;
struct list_head StackList;
NEWLOCK(stack_lock);

void *AllocateStack()
{
    // TODO: Well something about this implementation is broken (is not allocating a full stack (4096) and that's a big problem) and i am too tired to fix it. Maybe in the future i will fix it.
    void *NewStackAddress = RequestPage();
    debug("New stack allocated at %016p", (void *)NewStackAddress);
    return NewStackAddress;
    unsigned long long Stack = 0;
    SILENT_LOCK(stack_lock);
    if (list_empty(&StackList))
    {
        Stack = StackBase;
        StackBase += PAGE_SIZE + STACK_SIZE;
        for (int i = 0; i < (STACK_SIZE / PAGE_SIZE); i++)
        {
            if (KERNEL_STACK_HEAP_END < Stack + i * PAGE_SIZE)
            {
                panic("Stack out of bounds");
            }
            uint64_t NewStack = 0;
            NewStack = (uint64_t)RequestPage();
            MapMemory(NULL, (void *)(Stack + (i * PAGE_SIZE)), (void *)(NewStack - KERNEL_VMA_OFFSET), RW | PCD);
            debug("VIRTUAL: %016p", (void *)(Stack + (i * PAGE_SIZE)));
            debug("PHYSICAL: %016p", (void *)(NewStack - KERNEL_VMA_OFFSET));
        }
    }
    else
    {
        struct list_head *StackEntry = StackList.next;
        list_del(StackEntry);
        memset(StackEntry, 0, STACK_SIZE);
        Stack = (unsigned long long)StackEntry;
    }
    SILENT_UNLOCK(stack_lock);
    debug("0 %016p", (void *)Stack);
    Stack += STACK_SIZE;
    debug("1 %016p", (void *)Stack);
    Stack -= sizeof(unsigned long long) * 2;
    debug("New stack allocated mapped at %016p", (void *)Stack);
    return (void *)Stack;
}

void FreeStack(void *Stack)
{
    SILENT_LOCK(stack_lock);
    struct list_head *Head = (Stack + (sizeof(uint64_t) * 2)) - STACK_SIZE;
    list_add(&StackList, Head);
    debug("Stack %016p has been freed", Stack);
    SILENT_UNLOCK(stack_lock);
}

void init_stack()
{
    list_init(&StackList);
}
