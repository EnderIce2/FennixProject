#include "mz.hpp"

#include <filesystem.h>
#include <msexec.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "exec.hpp"

using namespace Tasking;
using namespace FileSystem;

RetStructData ExecuteMZ(const char *Path, ELEVATION Elevation, VMM::PageTableManager ptm)
{
    FILE *file = vfs->Open(Path);
    if (file->Status != FILESTATUS::OK || file->Node->Flags != NodeFlags::FS_FILE)
    {
        err("File status error %d for file %s", file->Status, Path);
        vfs->Close(file);
        return {0};
    }

    void *FileBuffer = KernelAllocator.RequestPages(file->Node->Length / PAGE_SIZE + 1);

    vfs->Read(file, 0, (uint8_t *)FileBuffer, file->Node->Length);

    IMAGE_DOS_HEADER *MZHeader = (IMAGE_DOS_HEADER *)FileBuffer;
    debug("32 bit MZ file found.");
    if (Elevation == ELEVATION::User)
    {
        uint64_t MappedAddrs = (uint64_t)FileBuffer;
        for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
        {
            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
            MappedAddrs += PAGE_SIZE;
        }
    }
    else if (Elevation == ELEVATION::Kernel)
    {
        uint64_t MappedAddrs = (uint64_t)FileBuffer;
        for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
        {
            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
            MappedAddrs += PAGE_SIZE;
        }
    }
    IMAGE_SECTION_HEADER *section = (IMAGE_SECTION_HEADER *)(((char *)MZHeader) + sizeof(IMAGE_DOS_HEADER));
    fixme("NumOfSections: %ld | SizeOfRawData: %ld",
          MZHeader->e_lfanew, section->SizeOfRawData);
    // if (section->SizeOfRawData == 0)
    //     continue;
    void *addr = (void *)((uint64_t)section->VirtualAddress + (uint64_t)FileBuffer);
    void *offset = KernelAllocator.RequestPages((uint64_t)addr / PAGE_SIZE + 1);
    if (Elevation == ELEVATION::User)
    {
        uint64_t MappedAddrs = (uint64_t)offset;
        for (uint64_t i = 0; i < (uint64_t)addr / PAGE_SIZE + 1; i++)
        {
            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
            MappedAddrs += PAGE_SIZE;
        }
    }
    else
    {
        uint64_t MappedAddrs = (uint64_t)offset;
        for (uint64_t i = 0; i < (uint64_t)addr / PAGE_SIZE + 1; i++)
        {
            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
            MappedAddrs += PAGE_SIZE;
        }
    }
    // memcpy(addr, ((char *)FileBuffer) + section->PointerToRawData, section->SizeOfRawData);
    debug("%s Entry Point: %#llx", Path, (uint64_t)(MZHeader->e_ip + (uint64_t)FileBuffer));
    return {(uint64_t)offset, MZHeader->e_ip};

Cleanup:
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / PAGE_SIZE + 1);
    vfs->Close(file);
    return {0};
}