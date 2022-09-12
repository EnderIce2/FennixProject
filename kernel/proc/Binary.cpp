#include "binary.hpp"

#include <internal_task.h>
#include <filesystem.h>
#include <critical.hpp>
#include <msexec.h>
#include <debug.h>
#include <heap.h>
#include <elf.h>

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

    void *FileBuffer = KernelAllocator.RequestPages(file->Node->Length / 0x1000 + 1);

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
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
    vfs->Close(file);
    return type;
}

PCB *ExecuteBinary(const char *Path, uint64_t Arg0, uint64_t Arg1, ELEVATION Elevation)
{
    EnterCriticalSection;
    FILE *file = vfs->Open(Path);
    BinType type = GetBinaryType(Path);
    if (file->Status != FILESTATUS::OK || file->Node->Flags != NodeFlags::FS_FILE)
    {
        err("File status error %d for file %s", file->Status, Path);
        vfs->Close(file);
        return nullptr;
    }

    void *FileBuffer = KernelAllocator.RequestPages(file->Node->Length / 0x1000 + 1);

    switch (type)
    {
    case BinType::BIN_TYPE_ELF:
    {
        vfs->Read(file, 0, (uint8_t *)FileBuffer, file->Node->Length);
        Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)FileBuffer;

        if (ELFHeader->e_ident[EI_CLASS] == ELFCLASS32)
        {
            err("32 bit ELF file not supported for now.");
            goto Cleanup;
        }
        if (ELFHeader->e_ident[EI_CLASS] == ELFCLASS64)
        {
            if (ELFHeader->e_type == ET_EXEC)
            {
                debug("64bit");
                if (ELFHeader->e_ident[EI_OSABI] == ELFOSABI_NONE)
                {
                    uint64_t MappedAddrs = (uint64_t)FileBuffer;
                    switch (Elevation)
                    {
                    case ELEVATION::User:
                    {
                        for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                        {
                            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                            MappedAddrs += PAGE_SIZE;
                        }
                        break;
                    }
                    case ELEVATION::Idle:
                    case ELEVATION::System:
                    case ELEVATION::Kernel:
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

                    Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader->e_phoff);
                    void *addr;
                    for (int i = 0; i < ELFHeader->e_phnum; i++, pheader++)
                    {
                        if (pheader->p_type != PT_LOAD)
                            continue;
                        addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
                    }
                    void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);
                    MappedAddrs = (uint64_t)offset;

                    switch (Elevation)
                    {
                    case ELEVATION::User:
                    {
                        for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                        {
                            KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                            MappedAddrs += PAGE_SIZE;
                        }
                        break;
                    }
                    case ELEVATION::Idle:
                    case ELEVATION::System:
                    case ELEVATION::Kernel:
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

                    pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader->e_phoff);
                    for (int i = 0; i < ELFHeader->e_phnum; i++, pheader++)
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
                          (uint64_t)(ELFHeader->e_entry + (uint64_t)offset),
                          ELFHeader->e_entry, offset);
                    vfs->Close(file);
                    LeaveCriticalSection;
                    if (CurrentTaskingMode == TaskingMode::Mono)
                    {
                        bool user = false;
                        if (Elevation == ELEVATION::User)
                            user = true;
                        return ConvertTaskCBToPCB(monot->CreateTask(ELFHeader->e_entry + (uint64_t)offset, Arg0, Arg1, (char *)file->Name, user, true));
                    }
                    else
                    {
                        PCB *pcb = SysCreateProcess(file->Name, Elevation);
                        pcb->Offset = (uint64_t)offset;
                        return SysCreateThread(pcb, (uint64_t)ELFHeader->e_entry, Arg0, Arg1)->Parent;
                    }
                }
                else
                {
                    fixme("File loading from other operating systems is not supported! (%d)", ELFHeader->e_ident[EI_OSABI]);
                    goto Cleanup;
                }
            }
            else if (ELFHeader->e_type == ET_DYN)
            {
                err("Dynamic ELF file not supported for now.");
                goto Cleanup;
            }
            else
            {
                err("Unknown ELF file type.");
                goto Cleanup;
            }
        }
        break;
    }
    case BinType::BIN_TYPE_PE:
    {
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
            if (Elevation == ELEVATION::User)
            {
                uint64_t MappedAddrs = (uint64_t)FileBuffer;
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            else if (Elevation == ELEVATION::Kernel)
            {
                uint64_t MappedAddrs = (uint64_t)FileBuffer;
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
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
                void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);
                if (Elevation == ELEVATION::User)
                {
                    uint64_t MappedAddrs = (uint64_t)offset;
                    for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                    {
                        KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                        MappedAddrs += PAGE_SIZE;
                    }
                }
                else if (Elevation == ELEVATION::Kernel)
                {
                    uint64_t MappedAddrs = (uint64_t)offset;
                    for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                    {
                        KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                        MappedAddrs += PAGE_SIZE;
                    }
                }
                memcpy(addr, ((char *)FileBuffer) + section->PointerToRawData, section->SizeOfRawData);
            }
            debug("%s Entry Point: %#llx", Path, (uint64_t)(PEHeader->OptionalHeader.AddressOfEntryPoint + (uint64_t)FileBuffer));
        }
        else
        {
            err("Unknown Machine: %#lx", PEHeader->FileHeader.Machine);
            goto Cleanup;
        }
        break;
    }
    case BinType::BIN_TYPE_NE:
    {
        warn("USING EXPERIMENTAL NE FILE LOADER!");
        IMAGE_DOS_HEADER *MZHeader = (IMAGE_DOS_HEADER *)FileBuffer;
        IMAGE_OS2_HEADER *NEHeader = (IMAGE_OS2_HEADER *)(((char *)FileBuffer) + MZHeader->e_lfanew);

        if (NEHeader->ne_exetyp == 0x02)
        {
            debug("32 bit NE file found.");
            if (Elevation == ELEVATION::User)
            {
                uint64_t MappedAddrs = (uint64_t)FileBuffer;
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            else if (Elevation == ELEVATION::Kernel)
            {
                uint64_t MappedAddrs = (uint64_t)FileBuffer;
                for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            IMAGE_SECTION_HEADER *section = (IMAGE_SECTION_HEADER *)(((char *)NEHeader) + sizeof(IMAGE_OS2_HEADER));
            for (int i = 0; i < NEHeader->ne_cbnrestab; i++, section++)
            {
                fixme("NumOfSections: %ld | SizeOfRawData: %ld",
                      NEHeader->ne_cbnrestab, section->SizeOfRawData);
                if (section->SizeOfRawData == 0)
                    continue;
                void *addr = (void *)((uint64_t)section->VirtualAddress + (uint64_t)FileBuffer);
                void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);
                if (Elevation == ELEVATION::User)
                {
                    uint64_t MappedAddrs = (uint64_t)offset;
                    for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                    {
                        KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                        MappedAddrs += PAGE_SIZE;
                    }
                }
                else
                {
                    uint64_t MappedAddrs = (uint64_t)offset;
                    for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                    {
                        KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                        MappedAddrs += PAGE_SIZE;
                    }
                }

                memcpy(addr, ((char *)FileBuffer) + section->PointerToRawData, section->SizeOfRawData);
            }
            debug("%s Entry Point: %#llx", Path, (uint64_t)(NEHeader->ne_enttab + (uint64_t)FileBuffer));
        }
        else
        {
            err("Unknown exe type: %#lx", NEHeader->ne_exetyp);
            goto Cleanup;
        }

        break;
    }
    case BinType::BIN_TYPE_MZ:
    {
        warn("USING EXPERIMENTAL MZ FILE LOADER!");
        IMAGE_DOS_HEADER *MZHeader = (IMAGE_DOS_HEADER *)FileBuffer;

        debug("32 bit MZ file found.");
        if (Elevation == ELEVATION::User)
        {
            uint64_t MappedAddrs = (uint64_t)FileBuffer;
            for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
            {
                KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                MappedAddrs += PAGE_SIZE;
            }
        }
        else if (Elevation == ELEVATION::Kernel)
        {
            uint64_t MappedAddrs = (uint64_t)FileBuffer;
            for (uint64_t i = 0; i < file->Node->Length / 0x1000 + 1; i++)
            {
                KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                MappedAddrs += PAGE_SIZE;
            }
        }
        IMAGE_SECTION_HEADER *section = (IMAGE_SECTION_HEADER *)(((char *)MZHeader) + sizeof(IMAGE_DOS_HEADER));
        for (int i = 0; i < MZHeader->e_lfanew; i++, section++)
        {
            fixme("NumOfSections: %ld | SizeOfRawData: %ld",
                  MZHeader->e_lfanew, section->SizeOfRawData);
            if (section->SizeOfRawData == 0)
                continue;
            void *addr = (void *)((uint64_t)section->VirtualAddress + (uint64_t)FileBuffer);
            void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);
            if (Elevation == ELEVATION::User)
            {
                uint64_t MappedAddrs = (uint64_t)offset;
                for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW | PTFlag::US);
                    MappedAddrs += PAGE_SIZE;
                }
            }
            else
            {
                uint64_t MappedAddrs = (uint64_t)offset;
                for (uint64_t i = 0; i < (uint64_t)addr / 0x1000 + 1; i++)
                {
                    KernelPageTableManager.MapMemory((void *)MappedAddrs, (void *)MappedAddrs, PTFlag::RW);
                    MappedAddrs += PAGE_SIZE;
                }
            }

            memcpy(addr, ((char *)FileBuffer) + section->PointerToRawData, section->SizeOfRawData);
        }
        debug("%s Entry Point: %#llx", Path, (uint64_t)(MZHeader->e_ip + (uint64_t)FileBuffer));

        break;
    }
    default:
        break;
    }
Cleanup:
    KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
    vfs->Close(file);
    LeaveCriticalSection;
    return nullptr;
}
