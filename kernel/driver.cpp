#include "driver.hpp"

#include <filesystem.h>
#include <task.h>
#include <heap.h>
#include <elf.h>

Driver::KernelDriver *kdrv = nullptr;

namespace Driver
{
    ProcessControlBlock *DrvMgrProc = nullptr;
    FileSystem::FileSystemNode *DriverNode = nullptr;

    uint64_t KernelDriver::LoadKernelDriver(FileSystem::FileSystemNode *Node)
    {
        void *DriverBuffer = KernelAllocator.RequestPages(Node->Length / 0x1000 + 1);
        FileSystem::FILE *driverfile = vfs->ConvertNodeToFILE(Node);
        vfs->Read(driverfile, 0, DriverBuffer, Node->Length);
        delete driverfile;
        Elf64_Ehdr *header = (Elf64_Ehdr *)DriverBuffer;
        if (header->e_ident[EI_MAG0] != ELFMAG0)
        {
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return 0xDE0;
        }
        if (header->e_ident[EI_MAG1] != ELFMAG1)
        {
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return 0xDE1;
        }
        if (header->e_ident[EI_MAG2] != ELFMAG2)
        {
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return 0xDE2;
        }
        if (header->e_ident[EI_MAG3] != ELFMAG3)
        {
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return 0xDE3;
        }
        if (header->e_ident[EI_CLASS] == ELFCLASS32)
        {
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return 0xDEC32;
        }
        if (header->e_ident[EI_CLASS] == ELFCLASS64)
        {
            Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)DriverBuffer) + header->e_phoff);
            void *addr;
            for (int i = 0; i < header->e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
            }
            void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);

            pheader = (Elf64_Phdr *)(((char *)DriverBuffer) + header->e_phoff);
            for (int i = 0; i < header->e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                void *dst = (void *)((uint64_t)pheader->p_vaddr + (uint64_t)offset);
                memset(dst, 0, pheader->p_memsz);
                memcpy(dst, ((char *)DriverBuffer) + pheader->p_offset, pheader->p_filesz);
            }
            // process pages -> addr / 0x1000 + 1;
            // KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            debug("Driver %s Entry Point: %#llx", Node->Name, (uint64_t)(header->e_entry + (uint64_t)offset));
            ThreadControlBlock *thread = SysCreateThread(reinterpret_cast<ProcessControlBlock *>(DrvMgrProc), (uint64_t)(header->e_entry + (uint64_t)offset), false);
            if (thread != nullptr)
            {
                // TODO: unloaded drivers need to be removed
                FileSystem::FileSystemNode *drvnode = vfs->Create(DriverNode, Node->Name);
                drvnode->Mode = 000;
                drvnode->IndexNode = thread->ThreadID;
                drvnode->Address = (uint64_t)(header->e_entry + (uint64_t)offset);
                drvnode->Flags = FileSystem::NodeFlags::FS_PIPE;
                return 0;
            }
            else
                return 0xFC59;
            // return 0xDEC64;
        }
        return 0;
    }

    ReadFSFunction(DriverRead)
    {
        return Size;
    }

    WriteFSFunction(DriverWrite)
    {
        return Size;
    }

    FileSystem::FileSystemOpeations kdrvfs = {
        .Name = "System Drivers",
        .Read = DriverRead,
        .Write = DriverWrite};

    KernelDriver::KernelDriver()
    {
        DrvMgrProc = SysCreateProcess("Driver Manager", nullptr);
        DriverNode = vfs->CreateRoot(&kdrvfs, "driver");
    }

    KernelDriver::~KernelDriver()
    {
        // TODO: kill driver manager process
    }
}
