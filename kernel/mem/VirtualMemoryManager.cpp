#include <heap.h>
#include <asm.h>
#include "../kernel.h"
#include "../cpu/cpuid.h"
#include "../cpu/smp.hpp"

using namespace PMM;
using namespace VMM;

PageTableManager KernelPageTableManager = NULL;

void PageTableManager::MapMemory(void *VirtualAddress, void *PhysicalAddress, uint64_t Flags, PageTable *PageTable4)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
    if (!this->PML4 && !PageTable4)
    {
        err("PML4 is null!");
        CPU_HALT;
    }
    if (PageTable4)
        PDE = PageTable4->Entries[indexer.PDP_i];
    else
        PDE = this->PML4->Entries[indexer.PDP_i];
    PageTable *PDP;
    if (!PDE.GetFlag(PTFlag::P))
    {
        PDP = (PageTable *)KernelAllocator.RequestPage();
        memset(PDP, 0, PAGE_SIZE);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PTFlag::P, true);
        PDE.AddFlag(Flags);
        if (PageTable4)
            PDE = PageTable4->Entries[indexer.PDP_i];
        else
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
        PDE.AddFlag(Flags);
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
        PDE.AddFlag(Flags);
        PD->Entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }
    PDE = PT->Entries[indexer.P_i];
    PDE.SetAddress((uint64_t)PhysicalAddress >> 12);
    PDE.SetFlag(PTFlag::P, true);
    PDE.AddFlag(Flags);
    PT->Entries[indexer.P_i] = PDE;
    invlpg((uint64_t)VirtualAddress);
}

void PageTableManager::UnmapMemory(void *VirtualAddress, PageTable *PageTable4)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
    if (PageTable4)
        PDE = PageTable4->Entries[indexer.PDP_i];
    else
        PDE = this->PML4->Entries[indexer.PDP_i];
    PDE.ClearFlags();
    // TODO: Free the allocated pages
    invlpg((uint64_t)VirtualAddress);
}

// https://wiki.osdev.org/Supervisor_Memory_Protection
void *PageTableManager::umemcpy(void *Destination, void *Source, uint64_t Length, enum CopyOperation Operation)
{
    // if (cpu_feature(CPUID_FEAT_RDX_UMIP))
    // if (cpu_feature(CPUID_FEAT_RDX_SMEP))

    // TODO: Check if the memory is copied to the right location
    if (Operation == CopyOperation::FromUser)
    {
        fixme("Copy from user");
    }
    else if (Operation == CopyOperation::ToUser)
    {
        fixme("Copy to user");
    }

    if (cpu_feature(CPUID_FEAT_RDX_SMAP))
        stac();

    void *ret = memcpy(Destination, Source, Length);

    if (cpu_feature(CPUID_FEAT_RDX_SMAP))
        clac();
    return ret;
}

void MapMemory(void *PML4, void *VirtualMemory, void *PhysicalMemory, uint64_t Flags)
{
    if (Flags == 0)
        KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, PTFlag::RW, (PageTable *)PML4);
    else
        KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, Flags, (PageTable *)PML4);
}

void init_vmm()
{
    trace("initializing virtual memory manager");
    KernelPageTableManager.Initalized = true;
}
