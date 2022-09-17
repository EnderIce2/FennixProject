#include <heap.h>
#include <asm.h>
#include <lock.h>
#include "../kernel.h"
#include "../cpu/idt.h"
#include "../cpu/smp.hpp"

using namespace PMM;
using namespace VMM;

NEWLOCK(pagetable_lock);

PageTable *KernelPML4;
PageTableHeap::PageTableHeap *KernelPageTableAllocator = nullptr;

namespace PageTableHeap
{
    static uint64_t MemoryEntries = bootparams->mem.Size;
    CR3 PageTableHeap::CreatePageTable(bool User)
    {
        LOCK(pagetable_lock);
        // PageTable *NewPML = (PageTable *)KernelAllocator.RequestPage();
        // PageTableManager NewPMLMgr = PageTableManager(NewPML);
        // memset(NewPML, 0, PAGE_SIZE);
        // for (uint64_t i = 256; i < 512; i++)
        //     NewPML->Entries[i].Value = KernelPML4->Entries[i].Value;

        // if (!User)
        // {
        // TODO: do something about this and map only where the process is created.
        // uint64_t VirtualOffsetNormalVMA = NORMAL_VMA_OFFSET;
        // for (uint64_t t = 0; t < MemoryEntries; t += PAGE_SIZE)
        // {
        //     NewPMLMgr.MapMemory((void *)t, (void *)t, PTFlag::RW);
        //     NewPMLMgr.MapMemory((void *)VirtualOffsetNormalVMA, (void *)t, PTFlag::RW);
        //     VirtualOffsetNormalVMA += PAGE_SIZE;
        // }
        // }

        CR3 cr;
        // cr.raw = (uint64_t)NewPML;
        cr.raw = (uint64_t)KernelPML4; // TODO: there is an issue with the code above. I should fix it ASAP.
        debug("New page table allocated at %#lx", cr.raw);
        UNLOCK(pagetable_lock);
        return cr;
    }

    void PageTableHeap::RemovePageTable(PageTable *PageTable)
    {
        // LOCK(pagetable_lock);
        // KernelAllocator.FreePage((void *)PageTable);
        // UNLOCK(pagetable_lock);
        // trace("Page table freed at %p", PageTable);
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

        uint64_t KernelStart = (uint64_t)&_kernel_start;
        uint64_t KernelTextEnd = (uint64_t)&_kernel_text_end;
        uint64_t KernelRoDataEnd = (uint64_t)&_kernel_rodata_end;
        uint64_t KernelEnd = (uint64_t)&_kernel_end;

        for (uint64_t t = 0; t < earlyparams.mem.Size; t += PAGE_SIZE)
        {
            // FIXME: US flag will be removed in future
            KernelPageTableManager.MapMemory((void *)t, (void *)t, PTFlag::RW | US);
            KernelPageTableManager.MapMemory((void *)VirtualOffsetNormalVMA, (void *)t, PTFlag::RW | US);
            VirtualOffsetNormalVMA += PAGE_SIZE;
        }

        /* Mapping Framebuffer address */
        for (uint64_t fb_base = earlyparams.Framebuffer.BaseAddress;
             fb_base < (earlyparams.Framebuffer.BaseAddress + (earlyparams.Framebuffer.BufferSize + PAGE_SIZE));
             fb_base += PAGE_SIZE)
            KernelPageTableManager.MapMemory((void *)fb_base, (void *)(fb_base - NORMAL_VMA_OFFSET), PTFlag::RW | PTFlag::US);

        /* Kernel mapping */
        for (uint64_t k = KernelStart; k < KernelTextEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::RW);
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

        TRACE_PML4(KernelPML4);

        /*    KernelStart             KernelTextEnd       KernelRoDataEnd                  KernelEnd
        Kernel Start & Text Start ------ Text End ------ Kernel Rodata End ------ Kernel Data End & Kernel End
        */
        trace("Applying new page table from address %#llx", KernelPML4);
        CR3 CR3PageTable;
        CR3PageTable.raw = (uint64_t)KernelPML4;
        writecr3(CR3PageTable);
        CurrentCPU->PageTable.raw = CR3PageTable.raw;
        SetKernelPageTableAddress((void *)KernelPML4);
    }
}
