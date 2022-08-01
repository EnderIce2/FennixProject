#include <heap.h>
#include <asm.h>
#include "../kernel.h"
#include "../cpu/smp.hpp"

using namespace PMM;
using namespace VMM;

PageTableManager KernelPageTableManager = NULL;

void PageTableManager::MapMemory(void *VirtualAddress, void *PhysicalAddress, uint64_t Flags)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
    if (!this->PML4)
    {
        err("PML4 is null!");
        CPU_HALT;
    }
    PDE = this->PML4->Entries[indexer.PDP_i];
    PageTable *PDP;
    if (!PDE.GetFlag(PTFlag::P))
    {
        PDP = (PageTable *)KernelAllocator.RequestPage();
        memset(PDP, 0, PAGE_SIZE);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PTFlag::P, true);
        PDE.AddFlag(Flags);
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

void PageTableManager::UnmapMemory(void *VirtualAddress)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)VirtualAddress);
    PageDirectoryEntry PDE;
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
    if (PML4 != NULL)
    {
        PageTable *KernelNewPML = KernelPageTableManager.PML4;
        KernelPageTableManager.PML4 = (PageTable *)PML4;
        if (Flags == 0)
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, PTFlag::RW);
        else
            KernelPageTableManager.MapMemory(VirtualMemory, PhysicalMemory, Flags);
        KernelPageTableManager.PML4 = KernelNewPML;
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
    KernelPageTableManager.Initalized = true;
}
