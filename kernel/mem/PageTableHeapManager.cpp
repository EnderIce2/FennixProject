#include <heap.h>
#include <asm.h>
#include <lock.h>
#include "../kernel.h"

using namespace PMM;
using namespace VMM;

NEWLOCK(pagetable_lock);

PageTable *KernelPML4;
PageTableHeap::PageTableHeap *KernelPageTableAllocator = nullptr;

namespace PageTableHeap
{
    PageTable *PageTableHeap::NewPageTable()
    {
        PageTable *ReturnHeap = (PageTable *)KernelAllocator.RequestPage();
        memset(ReturnHeap, 0, PAGE_SIZE);
        for (uint64_t i = 0; i < 512; i++)
            ReturnHeap->Entries[i] = KernelPML4->Entries[i];
        trace("New page table allocated at %p", ReturnHeap);
        return ReturnHeap;
    }

    void PageTableHeap::FreePageTable(PageTable *PageTable)
    {
        KernelAllocator.FreePage((void *)PageTable);
        trace("Page table freed at %p", PageTable);
    }

    PageTableHeap::PageTableHeap() { trace("Initialized."); }
    PageTableHeap::~PageTableHeap() { err("PageTableHeap destructor called"); }
}

void init_kernelpml()
{
    if (KernelPML4 == NULL)
    {
        debug("Kernel VM Start-End: %016p-%016p", &_kernel_start, &_kernel_end);
        KernelPML4 = (PageTable *)KernelAllocator.RequestPage();
        memset(KernelPML4, 0, PAGE_SIZE);
        KernelPageTableManager = PageTableManager(KernelPML4);

        uint64_t VirtualOffsetNormalVMA = NORMAL_VMA_OFFSET;
        uint64_t BaseKernelMapAddress = earlyparams.mem.KernelBasePhysical;

        uint64_t KernelStart = (uint64_t)ALIGN_UP(&_kernel_start, PAGE_SIZE);
        uint64_t KernelTextEnd = (uint64_t)ALIGN_UP(&_kernel_text_end, PAGE_SIZE);
        uint64_t KernelRoDataEnd = (uint64_t)ALIGN_UP(&_kernel_rodata_end, PAGE_SIZE);
        uint64_t KernelEnd = (uint64_t)ALIGN_UP(&_kernel_end, PAGE_SIZE);

        for (uint64_t t = 0; t < earlyparams.mem.Size; t += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)t, (void *)t, PTFlag::RW);
            KernelPageTableManager.MapMemory((void *)VirtualOffsetNormalVMA, (void *)t, PTFlag::RW);
            VirtualOffsetNormalVMA += PAGE_SIZE;
        }

        /* Mapping Framebuffer address */
        for (uint64_t fb_base = earlyparams.Framebuffer->BaseAddress;
             fb_base < (earlyparams.Framebuffer->BaseAddress + (earlyparams.Framebuffer->BufferSize + PAGE_SIZE));
             fb_base += PAGE_SIZE)
            KernelPageTableManager.MapMemory((void *)fb_base, (void *)(fb_base - NORMAL_VMA_OFFSET), PTFlag::RW);

        /* Kernel mapping */
        for (uint64_t k = KernelStart; k < KernelTextEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::US);
            KernelAllocator.LockPage((void *)BaseKernelMapAddress);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        for (uint64_t k = KernelTextEnd; k < KernelRoDataEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::P);
            KernelAllocator.LockPage((void *)BaseKernelMapAddress);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        for (uint64_t k = KernelRoDataEnd; k < KernelEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::RW);
            KernelAllocator.LockPage((void *)BaseKernelMapAddress);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        debug("\nStart: %#llx - Text End: %#llx - RoEnd: %#llx - End: %#llx\nStart Physical: %#llx - End Physical: %#llx",
              KernelStart, KernelTextEnd, KernelRoDataEnd, KernelEnd, earlyparams.mem.KernelBasePhysical, BaseKernelMapAddress);

        /*    KernelStart             KernelTextEnd       KernelRoDataEnd                  KernelEnd
        Kernel Start & Text Start ------ Text End ------ Kernel Rodata End ------ Kernel Data End & Kernel End
        */
        trace("Aapplying new page table from address %#llx", KernelPML4);
        CR3 CR3PageTable;
        CR3PageTable.raw = (uint64_t)KernelPML4;
        writecr3(CR3PageTable);
    }
}
