#include "driver.hpp"

#include <internal_task.h>
#include <filesystem.h>
#include <display.h>
#include <stdarg.h>
#include <cwalk.h>
#include <heap.h>
#include <elf.h>
#include <int.h>
#include <asm.h>

#include "security/security.hpp"
#include "kernel.h"

#pragma GCC diagnostic ignored "-Wvarargs"

Driver::KernelDriver *kdrv = nullptr;

void *FunctionCallHandler(KernelCallType type, ...)
{
    int maxargs = 16;
    int argsn = 0;
    uint64_t ArgList[maxargs];

    va_list args;
    va_start(args, maxargs);
    while (argsn < maxargs)
    {
        ArgList[argsn] = va_arg(args, uint64_t);
        argsn++;
    }
    va_end(args);

    switch (type)
    {
    case KCALL_REQ_PAGE:
    {
        return KernelAllocator.RequestPage();
    }
    case KCALL_FREE_PAGE:
    {
        KernelAllocator.FreePage((void *)ArgList[0]);
        return 0;
    }
    case KCALL_KMALLOC:
    {
        return kmalloc(ArgList[0]);
    }
    case KCALL_KFREE:
    {
        kfree((void *)ArgList[0]);
        return 0;
    }
    case KCALL_KCALLOC:
    {
        return kcalloc(ArgList[0], ArgList[1]);
    }
    case KCALL_KCREALLOC:
    {
        return krealloc((void *)ArgList[0], ArgList[1]);
    }
    case KCALL_GET_BOOTPARAMS:
    {
        return (void *)&bootparams;
    }
    case KCALL_HOOK_INTERRUPT:
    {
        return (void *)RegisterInterrupt((INTERRUPT_HANDLER)ArgList[1], (InterruptVector)ArgList[0], false);
    }
    case KCALL_UNHOOK_INTERRUPT:
        break;
    case KCALL_END_OF_INTERRUPT:
    {
        EndOfInterrupt(ArgList[0]);
        return 0;
    }
    default:
        fixme("Call %d not implemented.", type);
        return (void *)deadbeef;
    }
    return (void *)badfennec;
}

namespace Driver
{
    PCB *DrvMgrProc = nullptr;
    FileSystem::FileSystemNode *DriverNode = nullptr;

    uint64_t KernelDriver::LoadKernelDriverFromFile(FileSystem::FileSystemNode *Node)
    {
        void *DriverBuffer = KernelAllocator.RequestPages(Node->Length / 0x1000 + 1);
        FileSystem::FILE *driverfile = vfs->ConvertNodeToFILE(Node);
        vfs->Read(driverfile, 0, (uint8_t *)DriverBuffer, Node->Length);
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

            Elf64_Shdr shstrtab;
            memcpy(&shstrtab, (void *)((uint64_t)DriverBuffer + (header->e_shoff + static_cast<uint64_t>(header->e_shstrndx) * header->e_shentsize)), sizeof(Elf64_Shdr));
            char *names = (char *)kmalloc(shstrtab.sh_size);
            memcpy(names, (void *)((uint64_t)DriverBuffer + (shstrtab.sh_offset)), shstrtab.sh_size);

            for (uint16_t i = 0; i < header->e_shnum; i++)
            {
                struct elf64_shdr section;
                memcpy(&section, (void *)((uint64_t)DriverBuffer + (header->e_shoff + (uint64_t)i * header->e_shentsize)), sizeof(elf64_shdr));
                if (!strcmp(&names[section.sh_name], ".driverdata"))
                {
                    kfree(names);
                    DriverDefinition def;
                    DriverKernelMainData kerndata = {.KFctCall = FunctionCallHandler};
                    memcpy(&def, (void *)((uint64_t)DriverBuffer + (section.sh_offset)), section.sh_size);
                    void *DriverStartFunction = (void *)(header->e_entry + (Elf64_Addr)offset);
                    debug("Driver Name: %s", def.Name);
                    debug("Entry:%#llx + Offset:%#llx -> %#llx", header->e_entry, offset, DriverStartFunction);
                    uint64_t ret = DRIVER_SUCCESS; // ((uint64_t(*)(DriverKernelMainData *))(DriverStartFunction))(&kerndata);
                    fixme("Driver will not be loaded.");
                    if (ret == DRIVER_SUCCESS)
                    {
                        // TODO: unloaded drivers need to be removed
                        FileSystem::FileSystemNode *drvnode = vfs->Create(DriverNode, Node->Name);
                        drvnode->Mode = 000;
                        drvnode->Address = (uint64_t)(header->e_entry + (uint64_t)offset);
                        drvnode->Flags = FileSystem::NodeFlags::FS_PIPE;
                        return 0;
                    }
                    else
                    {
                        KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
                        return 0xDF01;
                    }
                }
            }
            KernelAllocator.FreePages(DriverBuffer, Node->Length / 0x1000 + 1);
            kfree(names);
            return 0xDEC64;
        }
        return 0;
    }

    ReadFSFunction(DriverRead)
    {
        fixme("Not implemented.");
        return Size;
    }

    WriteFSFunction(DriverWrite)
    {
        fixme("Not implemented.");
        return Size;
    }

    FileSystem::FileSystemOpeations kdrvfs = {
        .Name = "System Drivers",
        .Read = DriverRead,
        .Write = DriverWrite};

    void DriverManagerMainLoop()
    {
        SysSetThreadPriority(100);
        // PCB *drvpcb = SysCreateProcessFromFile("/system/drvmgr", 0, 0, CBElevation::User);
        // if (!drvpcb)
        // {
        //     CurrentDisplay->SetPrintColor(0xFC4444);
        //     printf("Failed to load /system/drvmgr process. The file is missing or corrupted.\n");
        //     delete kdrv;
        //     CPU_HALT;
        // }

        // TrustToken(drvpcb->Security.Token, true, drvpcb->ID, TokenTrustLevel::TrustedByKernel);

        FileSystem::FILE *driverDirectory = vfs->Open("/system/drivers");
        if (driverDirectory->Status == FileSystem::FILESTATUS::OK)
        {
            foreach (auto driver in driverDirectory->Node->Children)
            {
                if (driver->Flags == FileSystem::NodeFlags::FS_FILE)
                    if (cwk_path_has_extension(driver->Name))
                    {
                        const char *extension;
                        cwk_path_get_extension(driver->Name, &extension, nullptr);

                        if (!strcmp(extension, ".drv"))
                        {
                            CurrentDisplay->SetPrintColor(0xCCCCCC);
                            printf("Loading driver %s... ", driver->Name);
                            uint64_t ret = kdrv->LoadKernelDriverFromFile(driver);
                            if (ret == 0)
                            {
                                CurrentDisplay->SetPrintColor(0x058C19);
                                printf("OK\n");
                            }
                            else
                            {
                                CurrentDisplay->SetPrintColor(0xE85230);
                                printf("FAILED (%#lx)\n", ret);
                            }
                            CurrentDisplay->ResetPrintColor();
                            // TODO: get instruction pointer of the elf entry point.
                        }
                    }
            }
        }
        vfs->Close(driverDirectory);
        SysSetThreadPriority(5);
        while (1)
        {
            // do IPC here.
            PAUSE;
        }
    }

    KernelDriver::KernelDriver()
    {
        drvfs = new FileSystem::Driver;
        drvfs->AddDriver(&kdrvfs, 0666, "default", FileSystem::NodeFlags::FS_PIPE);
        DrvMgrProc = SysCreateProcess("Driver Manager", CBElevation::System);
        DrvMgrThrd = SysCreateThread((PCB *)DrvMgrProc, (uint64_t)DriverManagerMainLoop, 0, 0);
    }

    KernelDriver::~KernelDriver()
    {
        // TODO: kill driver manager process
    }
}
