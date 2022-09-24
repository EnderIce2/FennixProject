#include "elf.hpp"

#include <filesystem.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "exec.hpp"

using namespace Tasking;
using namespace FileSystem;

RetStructData ExecuteELF(const char *Path, CBElevation Elevation, VMM::PageTableManager ptm)
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

    Elf64_Ehdr ELFHeader;
    memcpy(&ELFHeader, FileBuffer, sizeof(Elf64_Ehdr));

    if (ELFHeader.e_ident[EI_CLASS] == ELFCLASS32)
    {
        err("32 bit ELF file not supported for now.");
        goto Cleanup;
    }
    if (ELFHeader.e_ident[EI_CLASS] == ELFCLASS64)
    {
        if (ELFHeader.e_type == ET_EXEC ||
            ELFHeader.e_type == ET_DYN ||
            ELFHeader.e_type == ET_REL)
        {
            debug("64bit");

//             switch (Elevation)
//             {
//             case CBElevation::User:
//             {
//                 uint64_t BufferAddrs = (uint64_t)FileBuffer;
//                 uint64_t FileBufferAddress = USER_FILE_BUFFER_BASE;
//                 for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
//                 {
//                     ptm.MapMemory((void *)FileBufferAddress, (void *)BufferAddrs, PTFlag::RW | PTFlag::US);
//                     /* we can't access it while we are still inside the kernel page table */
//                     KernelPageTableManager.MapMemory((void *)FileBufferAddress, (void *)BufferAddrs, PTFlag::RW);
//                     debug("1-Mapping %p to %p", FileBufferAddress, BufferAddrs);
//                     FileBufferAddress += PAGE_SIZE;
//                     BufferAddrs += PAGE_SIZE;
//                 }
//                 break;
//             }
//             case CBElevation::Idle:
//             case CBElevation::System:
//             case CBElevation::Kernel:
//             {
//                 uint64_t BufferAddrs = (uint64_t)FileBuffer;
//                 for (uint64_t i = 0; i < file->Node->Length / PAGE_SIZE + 1; i++)
//                 {
//                     KernelPageTableManager.MapMemory((void *)BufferAddrs, (void *)BufferAddrs, PTFlag::RW);
//                     debug("1-Mapping %p to %p", BufferAddrs, BufferAddrs);
//                     BufferAddrs += PAGE_SIZE;
//                 }
//                 break;
//             }
//             default:
//             {
//                 err("Invalid Elevation");
//                 goto Cleanup;
//             }
//             }
//             Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader.e_phoff);
// #ifdef DEBUG
//             for (Elf64_Half i = 0; i < ELFHeader.e_phnum; i++, pheader++)
//             {
//                 debug("----------------------------------");
//                 debug("p_type: %#lx", pheader->p_type);
//                 debug("p_flags: %#lx", pheader->p_flags);
//                 debug("p_offset: %#lx", pheader->p_offset);
//                 debug("p_vaddr: %#lx", pheader->p_vaddr);
//                 debug("p_paddr: %#lx", pheader->p_paddr);
//                 debug("p_filesz: %#lx", pheader->p_filesz);
//                 debug("p_memsz: %#lx", pheader->p_memsz);
//                 debug("p_align: %#lx", pheader->p_align);
//             }
//             pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader.e_phoff);
// #endif
//             void *addr;
//             for (Elf64_Half i = 0; i < ELFHeader.e_phnum; i++, pheader++)
//             {
//                 if (pheader->p_type != PT_LOAD)
//                     continue;
//                 addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
//             }
//             if (!addr)
//                 goto Cleanup;
//             debug("Address: %#lx", addr);
//             void *offset = KernelAllocator.RequestPages((uint64_t)addr / PAGE_SIZE + 1);
//             switch (Elevation)
//             {
//             case CBElevation::User:
//             {
//                 uint64_t Base = USER_APP_BASE;
//                 uint64_t PhysicalBase = (uint64_t)offset;
//                 for (uint64_t i = 0; i < (uint64_t)addr / PAGE_SIZE + 1; i++)
//                 {
//                     ptm.MapMemory((void *)Base, (void *)PhysicalBase, PTFlag::RW | PTFlag::US);
//                     /* we can't access it while we are still inside the kernel page table */
//                     KernelPageTableManager.MapMemory((void *)Base, (void *)PhysicalBase, PTFlag::RW);
//                     // debug("2-Mapping %p to %p", Base, PhysicalBase);
//                     PhysicalBase += PAGE_SIZE;
//                     Base += PAGE_SIZE;
//                 }
//                 break;
//             }
//             case CBElevation::Idle:
//             case CBElevation::System:
//             case CBElevation::Kernel:
//             default:
//             {
//                 err("Invalid Elevation");
//                 goto Cleanup;
//             }
//             }
//             uint64_t AppBaseOffsetAddress = USER_APP_BASE;
//             if (Elevation == CBElevation::Kernel ||
//                 Elevation == CBElevation::Kernel ||
//                 Elevation == CBElevation::Idle)
//                 AppBaseOffsetAddress = (uint64_t)offset;
//             pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader.e_phoff);
//             for (Elf64_Half i = 0; i < ELFHeader.e_phnum; i++, pheader++)
//             {
//                 if (pheader->p_type != PT_LOAD)
//                     continue;
//                 void *dst = (void *)((uint64_t)pheader->p_vaddr + AppBaseOffsetAddress);
//                 memset(dst, 0, pheader->p_memsz);
//                 memcpy(dst, ((char *)FileBuffer) + pheader->p_offset, pheader->p_filesz);
//             }
//             // process pages -> addr / PAGE_SIZE + 1;
//             debug("%s Entry Point: %#lx [%#lx + %#lx]",
//                   Path,
//                   (uint64_t)(ELFHeader.e_entry + AppBaseOffsetAddress),
//                   ELFHeader.e_entry, AppBaseOffsetAddress);
//             vfs->Close(file);
//             RetStructData ret = {.Offset = AppBaseOffsetAddress, .Entry = ELFHeader.e_entry};
//             return ret;

            uint64_t MappedAddrs = (uint64_t)FileBuffer;
            switch (Elevation)
            {
            case CBElevation::User:
            {
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
                break;
            }
            case CBElevation::Idle:
            case CBElevation::System:
            case CBElevation::Kernel:
            {
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            default:
            {
                err("Invalid Elevation");
                goto Cleanup;
            }
            }
            Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader.e_phoff);
            void *addr;
            for (int i = 0; i < ELFHeader.e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
            }
            void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);
            MappedAddrs = (uint64_t)offset;
            switch (Elevation)
            {
            case CBElevation::User:
            {
                for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
                break;
            }
            case CBElevation::Idle:
            case CBElevation::System:
            case CBElevation::Kernel:
            {
                for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                    MappedAddrs += PAGE_SIZE;
                }
                break;
            }
            default:
            {
                err("Invalid Elevation");
                goto Cleanup;
            }
            }
            pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader.e_phoff);
            for (int i = 0; i < ELFHeader.e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                void *dst = (void *)((uint64_t)pheader->p_vaddr + (uint64_t)offset);
                memset(dst, 0, pheader->p_memsz);
                memcpy(dst, ((char *)FileBuffer) + pheader->p_offset, pheader->p_filesz);
            }
            // process pages -> addr / 0x1000 + 1;
            debug("%s Entry Point: %#lx [%#lx + %#lx]",
                  Path,
                  (uint64_t)(ELFHeader.e_entry + (uint64_t)offset),
                  ELFHeader.e_entry, offset);
            vfs->Close(file);
            RetStructData ret = {.Offset = (uint64_t)offset, .Entry = ELFHeader.e_entry};
            return ret;
        }
        else
        {
            err("Unknown ELF file type.");
            goto Cleanup;
        }
    }
Cleanup:
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / PAGE_SIZE + 1);
    vfs->Close(file);
    return {0};
}
