#include "pe.hpp"

#include <filesystem.h>
#include <msexec.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "exec.hpp"

using namespace Tasking;
using namespace FileSystem;

RetStructData ExecutePE(const char *Path, CBElevation Elevation, VMM::PageTableManager ptm)
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
    IMAGE_NT_HEADERS *PEHeader = (IMAGE_NT_HEADERS *)(((char *)FileBuffer) + MZHeader->e_lfanew);
    if (PEHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
    {
        err("32 bit PE file not supported for now.");
        goto Cleanup;
    }
    else if (PEHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
    {
        debug("64 bit PE file found.");
        if (Elevation == CBElevation::User)
        {
            uint64_t MappedAddrs = (uint64_t)FileBuffer;
            for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
            {
                KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                MappedAddrs += PAGE_SIZE;
            }
        }
        else if (Elevation == CBElevation::Kernel)
        {
            uint64_t MappedAddrs = (uint64_t)FileBuffer;
            for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
            {
                KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                MappedAddrs += PAGE_SIZE;
            }
        }
        IMAGE_SECTION_HEADER *section = (IMAGE_SECTION_HEADER *)(((char *)PEHeader) + sizeof(IMAGE_NT_HEADERS));
        for (int i = 0; i < PEHeader->FileHeader.NumberOfSections; i++, section++)
        {
            fixme("NumOfSections: %ld | SizeOfRawData: %ld",
                  PEHeader->FileHeader.NumberOfSections, section->SizeOfRawData);
            if (section->SizeOfRawData == 0)
                continue;
            void *addr = (void *)((uint64_t)section->VirtualAddress + (uint64_t)FileBuffer);
            void *offset = KernelAllocator.RequestPages((uint64_t)addr / PAGE_SIZE + 1);
            if (Elevation == CBElevation::User)
            {
                uint64_t MappedAddrs = (uint64_t)offset;
                for (uint64_t i = 0; i < (uint64_t)addr / PAGE_SIZE + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            else if (Elevation == CBElevation::Kernel)
            {
                uint64_t MappedAddrs = (uint64_t)offset;
                for (uint64_t i = 0; i < (uint64_t)addr / PAGE_SIZE + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            memcpy(addr, ((char *)FileBuffer) + section->PointerToRawData, section->SizeOfRawData);
        }
        debug("%s Entry Point: %#llx", Path, (uint64_t)(PEHeader->OptionalHeader.AddressOfEntryPoint + (uint64_t)FileBuffer));
        return {(uint64_t)FileBuffer, PEHeader->OptionalHeader.AddressOfEntryPoint};
    }
    else
    {
        err("Unknown Machine: %#lx", PEHeader->FileHeader.Machine);
        goto Cleanup;
    }
Cleanup:
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / PAGE_SIZE + 1);
    vfs->Close(file);
    return {0};
}