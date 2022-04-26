#include "driver.hpp"

#include <heap.h>
#include <elf.h>

Driver::KernelDriver *kdrv = nullptr;

namespace Driver
{
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
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            return deadbeef;
            // return 0xDEC64;
        }
        return 0;
    }

    KernelDriver::KernelDriver()
    {
    }

    KernelDriver::~KernelDriver()
    {
    }
}
