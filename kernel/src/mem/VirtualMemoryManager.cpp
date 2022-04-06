#include <heap.h>
#include <asm.h>
#include "../kernel.h"

using namespace PMM;
using namespace VMM;

PageTable *KernelPML4;
PageTableManager KernelPageTableManager = NULL;

void PageTableManager::MapMemory(void *VirtualAddress, void *PhysicalAddress, uint64_t Flags)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
#ifdef DEBUG
    static int once = 0;
    if (!once++)
        debug("VM: %016p, PM: %016p (PDP:%#x|PD:%#x|PT:%#x|P:%#x)", VirtualAddress, PhysicalAddress, indexer.PDP_i, indexer.PD_i, indexer.PT_i, indexer.P_i);
    if (once >= 100000)
    {
        once = 0;
    }
    else
        once++;
#endif
    PDE = this->PML4->Entries[indexer.PDP_i];
    PageTable *PDP;
    if (!PDE.GetFlag(PTFlag::P))
    {
        PDP = (PageTable *)KernelAllocator.RequestPage();
        memset(PDP, 0, PAGE_SIZE);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PTFlag::P, true);
        PDE.AddFlags(Flags);
        this->PML4->Entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }
    PDE = PDP->Entries[indexer.PD_i];
    PageTable *PD;
    if (!PDE.GetFlag(PTFlag::P))
    {
        PD = (PageTable *)KernelAllocator.RequestPage();
        memset(PD, 0, PAGE_SIZE);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PTFlag::P, true);
        PDE.AddFlags(Flags);
        PDP->Entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }
    PDE = PD->Entries[indexer.PT_i];
    PageTable *PT;
    if (!PDE.GetFlag(PTFlag::P))
    {
        PT = (PageTable *)KernelAllocator.RequestPage();
        memset(PT, 0, PAGE_SIZE);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PTFlag::P, true);
        PDE.AddFlags(Flags);
        PD->Entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }
    PDE = PT->Entries[indexer.P_i];
    PDE.SetAddress((uint64_t)PhysicalAddress >> 12);
    PDE.SetFlag(PTFlag::P, true);
    PDE.AddFlags(Flags);
    PT->Entries[indexer.P_i] = PDE;
    // invlpg((uint64_t)VirtualAddress);
}

void PageTableManager::UnmapMemory(void *VirtualAddress)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
    PDE = this->PML4->Entries[indexer.PDP_i];
    PDE.ClearFlags();
    // TODO: Free the allocated pages
    invlpg((uint64_t)VirtualAddress);
}

PageTable *CreateNewPML4()
{
    if (KernelPML4 == NULL)
    {
        debug("Kernel VM Start-End: %016p-%016p", &_kernel_start, &_kernel_end);
        // debug("HHDM Address: %016p", earlyparams.pmr.HHDMAddress);
        // debug("Kernel Address: V:%016p P:%016p", earlyparams.pmr.KernelVirtualAddress, earlyparams.pmr.KernelPhysicalAddress);
        // KernelPML4 = (PageTable *)readcr3();
        KernelPML4 = (PageTable *)KernelAllocator.RequestPage();
        memset(KernelPML4, 0, PAGE_SIZE);
        KernelPageTableManager = PageTableManager(KernelPML4);

        // // Reserved memory mapping (BIOS data)
        // for (uint64_t i = (uint64_t)ALIGN_UP(0x0, PAGE_SIZE); i < (uint64_t)ALIGN_UP(0x000FFFFF, PAGE_SIZE); i += PAGE_SIZE)
        // {
        //     KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW | PTFlag::PCD);
        // }
        // // Free to use RAM (14 MiB)
        // for (uint64_t i = (uint64_t)ALIGN_UP(0x00100000, PAGE_SIZE); i < (uint64_t)ALIGN_UP(0x00EFFFFF, PAGE_SIZE); i += PAGE_SIZE)
        // {
        //     KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW | PTFlag::PCD);
        // }
        // // Possible memory mapped hardware (1 MiB)
        // for (uint64_t i = (uint64_t)ALIGN_UP(0x00F00000, PAGE_SIZE); i < (uint64_t)ALIGN_UP(0x00FFFFFF, PAGE_SIZE); i += PAGE_SIZE)
        // {
        //     KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW | PTFlag::PCD);
        // }
        // // Memory mapped PCI devices, PnP NVRAM, IO APIC/s, local APIC/s, BIOS, ... (1 GiB)
        // for (uint64_t i = (uint64_t)ALIGN_UP(0xC0000000, PAGE_SIZE); i < (uint64_t)ALIGN_UP(0xFFFFFFFF, PAGE_SIZE); i += PAGE_SIZE)
        // {
        //     KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW | PTFlag::PCD);
        // }

        for (uint64_t t = 0; t < earlyparams.mem.Size; t += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)t, (void *)t, PTFlag::RW);
        }

        uint64_t VirtualOffsetNormalVMA = NORMAL_VMA_OFFSET;
        for (uint64_t t = 0; t < earlyparams.mem.Size; t += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)VirtualOffsetNormalVMA, (void *)t, PTFlag::RW);
            VirtualOffsetNormalVMA += PAGE_SIZE;
        }

        /* Kernel mapping */

        uint64_t BaseKernelMapAddress = earlyparams.mem.KernelBasePhysical;

        uintptr_t KernelStart = (uintptr_t)ALIGN_UP(&_kernel_start, PAGE_SIZE);
        uintptr_t KernelTextEnd = (uintptr_t)ALIGN_UP(&_kernel_text_end, PAGE_SIZE);
        uintptr_t KernelRoDataEnd = (uintptr_t)ALIGN_UP(&_kernel_rodata_end, PAGE_SIZE);
        uintptr_t KernelEnd = (uintptr_t)ALIGN_UP(&_kernel_end, PAGE_SIZE);

        for (uintptr_t k = KernelStart; k < KernelTextEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::US);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        for (uintptr_t k = KernelTextEnd; k < KernelRoDataEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::P);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        for (uintptr_t k = KernelRoDataEnd; k < KernelEnd; k += PAGE_SIZE)
        {
            KernelPageTableManager.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::RW);
            BaseKernelMapAddress += PAGE_SIZE;
        }

        debug("\nStart: %016p - Text End: %016p - RoEnd: %016p - End: %016p\nStart Physical: %016p - End Physical: %016p",
              KernelStart, KernelTextEnd, KernelRoDataEnd, KernelEnd, earlyparams.mem.KernelBasePhysical, BaseKernelMapAddress);

        /*    KernelStart             KernelTextEnd       KernelRoDataEnd                  KernelEnd
        Kernel Start & Text Start ------ Text End ------ Kernel Rodata End ------ Kernel Data End & Kernel End
        */

        return KernelPML4;
    }
    PageTable *NewPML4 = (PageTable *)KernelAllocator.RequestPage();
    memset(NewPML4, 0, PAGE_SIZE);
    for (size_t i = 0; i < 512; i++)
        NewPML4->Entries[i] = KernelPML4->Entries[i];
    return NewPML4;
}

void MapMemory(void *PML4, void *VirtualMemory, void *PhysicalMemory, uint64_t Flags)
{
    if (PML4 != NULL)
    {
        PageTable *curpml = KernelPageTableManager.PML4;
        KernelPageTableManager.PML4 = (PageTable *)PML4;
        if (Flags == 0)
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, PTFlag::RW);
        else
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, Flags);
        KernelPageTableManager.PML4 = curpml;
    }
    else
    {
        if (Flags == 0)
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, PTFlag::RW);
        else
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, Flags);
    }
}

void init_vmm()
{
    trace("initializing virtual memory manager");
    KernelPML4 = CreateNewPML4();
    debug("applying new page table with address %016p", reinterpret_cast<uintptr_t>(KernelPML4));
    writecr3((uint64_t)KernelPML4);
}
