#include "binary.hpp"

#include <internal_task.h>
#include <filesystem.h>
#include <critical.hpp>
#include <msexec.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "exec/exec.hpp"
#include "exec/elf.hpp"
#include "exec/pe.hpp"
#include "exec/ne.hpp"
#include "exec/mz.hpp"
#include "../kernel.h"

using namespace Tasking;
using namespace FileSystem;

BinType GetBinaryType(const char *Path)
{
    EnterCriticalSection;
    BinType type = BIN_TYPE_UNKNOWN;
    FILE *file = vfs->Open(Path);
    if (file->Status != FILESTATUS::OK || file->Node->Flags != NodeFlags::FS_FILE)
    {
        err("File status error %d for file %s", file->Status, Path);
        type = BinType::BIN_TYPE_UNKNOWN;
        vfs->Close(file);
        return type;
    }

    void *FileBuffer = KernelAllocator.RequestPages(file->Node->Length / PAGE_SIZE + 1);

    vfs->Read(file, 0, (uint8_t *)FileBuffer, file->Node->Length);
    Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)FileBuffer;
    IMAGE_DOS_HEADER *MZHeader = (IMAGE_DOS_HEADER *)FileBuffer;

    if (ELFHeader->e_ident[EI_MAG0] == ELFMAG0 &&
        ELFHeader->e_ident[EI_MAG1] == ELFMAG1 &&
        ELFHeader->e_ident[EI_MAG2] == ELFMAG2 &&
        ELFHeader->e_ident[EI_MAG3] == ELFMAG3)
    {
        trace("%s - ELF", Path);
        type = BinType::BIN_TYPE_ELF;
        goto Cleanup;
    }
    if (MZHeader->e_magic == IMAGE_DOS_SIGNATURE)
    {
        IMAGE_NT_HEADERS *PEHeader = (IMAGE_NT_HEADERS *)(((char *)FileBuffer) + MZHeader->e_lfanew);
        IMAGE_OS2_HEADER *NEHeader = (IMAGE_OS2_HEADER *)(((char *)FileBuffer) + MZHeader->e_lfanew);
        if (NEHeader->ne_magic == IMAGE_OS2_SIGNATURE)
        {
            trace("%s - NE", Path);
            type = BinType::BIN_TYPE_NE;
            goto Cleanup;
        }
        else if (PEHeader->Signature == IMAGE_NT_SIGNATURE)
        {
            trace("%s - PE", Path);
            type = BinType::BIN_TYPE_PE;
            goto Cleanup;
        }
        else
        {
            trace("%s - MZ", Path);
            type = BinType::BIN_TYPE_MZ;
            goto Cleanup;
        }
    }
Cleanup:
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / PAGE_SIZE + 1);
    vfs->Close(file);
    return type;
}

void CreatePageTableWithKernel(VMM::PageTableManager ptm)
{
    uint64_t KernelStart = (uint64_t)&_kernel_start;
    uint64_t KernelTextEnd = (uint64_t)&_kernel_text_end;
    uint64_t KernelRoDataEnd = (uint64_t)&_kernel_rodata_end;
    uint64_t KernelEnd = (uint64_t)&_kernel_end;

    uint64_t BaseKernelMapAddress = bootparams->mem.KernelBasePhysical;
    uint64_t VirtualOffsetNormalVMA = NORMAL_VMA_OFFSET;

    debug("Mapping Kernel PML4");
    ptm.MapMemory((void *)KernelPageTableManager.PML4, (void *)KernelPageTableManager.PML4, PTFlag::RW); // make sure that the kernel page table is mapped

    debug("Mapping memory");
    for (uint64_t t = 0; t < bootparams->mem.Size; t += PAGE_SIZE)
    {
        ptm.MapMemory((void *)t, (void *)t, PTFlag::RW | PTFlag::US);
        ptm.MapMemory((void *)VirtualOffsetNormalVMA, (void *)t, PTFlag::RW | PTFlag::US);
        VirtualOffsetNormalVMA += PAGE_SIZE;
    }

    /* Mapping Framebuffer address */
    debug("Mapping Framebuffer");
    for (uint64_t fb_base = bootparams->Framebuffer.BaseAddress;
         fb_base < (bootparams->Framebuffer.BaseAddress + (bootparams->Framebuffer.BufferSize + PAGE_SIZE));
         fb_base += PAGE_SIZE)
        ptm.MapMemory((void *)fb_base, (void *)(fb_base - NORMAL_VMA_OFFSET), PTFlag::RW | PTFlag::US);

    /* Kernel mapping */
    debug("Mapping Kernel Text");
    for (uint64_t k = KernelStart; k < KernelTextEnd; k += PAGE_SIZE)
    {
        ptm.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::RW);
        BaseKernelMapAddress += PAGE_SIZE;
    }

    debug("Mapping Kernel RoData");
    for (uint64_t k = KernelTextEnd; k < KernelRoDataEnd; k += PAGE_SIZE)
    {
        ptm.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::P);
        BaseKernelMapAddress += PAGE_SIZE;
    }

    debug("Mapping Kernel Data");
    for (uint64_t k = KernelRoDataEnd; k < KernelEnd; k += PAGE_SIZE)
    {
        ptm.MapMemory((void *)k, (void *)BaseKernelMapAddress, PTFlag::RW);
        BaseKernelMapAddress += PAGE_SIZE;
    }
}

PCB *ExecuteBinary(const char *Path, uint64_t Arg0, uint64_t Arg1, ELEVATION Elevation)
{
    EnterCriticalSection;
    BinType type = GetBinaryType(Path);

    debug("Creating new pml4...");
    VMM::PageTable *pml4 = (VMM::PageTable *)KernelAllocator.RequestPage();
    // memset(pml4, 0, PAGE_SIZE);
    VMM::PageTableManager ptm = VMM::PageTableManager(pml4);
    // CreatePageTableWithKernel(ptm);
    // TRACE_PML4(pml4);

    RetStructData ret;

    switch (type)
    {
    case BinType::BIN_TYPE_ELF:
    {
        ret = ExecuteELF(Path, Elevation, ptm);
        if (ret.Entry == 0)
            goto Cleanup;
        else
            goto Success;
    }
    case BinType::BIN_TYPE_PE:
    {
        ret = ExecutePE(Path, Elevation, ptm);
        if (ret.Entry == 0)
            goto Cleanup;
        else
            goto Success;
        break;
    }
    case BinType::BIN_TYPE_NE:
    {
        warn("USING EXPERIMENTAL NE FILE LOADER!");
        ret = ExecuteNE(Path, Elevation, ptm);
        if (ret.Entry == 0)
            goto Cleanup;
        else
            goto Success;
        break;
    }
    case BinType::BIN_TYPE_MZ:
    {
        warn("USING EXPERIMENTAL MZ FILE LOADER!");
        ret = ExecuteMZ(Path, Elevation, ptm);
        if (ret.Entry == 0)
            goto Cleanup;
        else
            goto Success;
        break;
    }
    default:
        break;
    }

Cleanup:
    KernelAllocator.FreePage(pml4);
    LeaveCriticalSection;
    return nullptr;

Success:
    debug("Entry: %#lx Offset: %#lx", ret.Entry, ret.Offset);
    LeaveCriticalSection;
    if (CurrentTaskingMode == TaskingMode::Mono)
    {
        bool user = false;
        if (Elevation == ELEVATION::User)
            user = true;
        FILE *file = vfs->Open(Path);
        Tasking::TaskControlBlock *task = monot->CreateTask(ret.Entry + ret.Offset, Arg0, Arg1, (char *)file->Name, user, true);
        vfs->Close(file);

        // if (Elevation != ELEVATION::Kernel && Elevation != ELEVATION::Idle)
        // {
        //     KernelAllocator.FreePage(task->pml4);
        //     task->pml4 = pml4;
        // }
        // else
        //     KernelAllocator.FreePage(pml4);
        return ConvertTaskCBToPCB(task);
    }
    else
    {
        FILE *file = vfs->Open(Path);
        PCB *pcb = SysCreateProcess(file->Name, Elevation);
        vfs->Close(file);
        pcb->Offset = ret.Offset;
        // if (Elevation != ELEVATION::Kernel && Elevation != ELEVATION::Idle)
        // {
        //     KernelAllocator.FreePage((void *)pcb->PageTable.raw);
        //     pcb->PageTable.raw = (uint64_t)pml4;
        // }
        // else
        //     KernelAllocator.FreePage(pml4);
        return SysCreateThread(pcb, ret.Entry, Arg0, Arg1)->Parent;
    }
}
