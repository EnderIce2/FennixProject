#include <internal_task.h>

#include <filesystem.h>
#include <critical.hpp>
#include <debug.h>
#include <heap.h>
#include <elf.h>
#include <msexec.h>

#include "../cpu/smp.hpp"

using namespace Tasking;
using namespace FileSystem;

int CurrentTaskingMode = TaskingMode::None;

PCB *nullpcb = nullptr;
TCB *nulltcb = nullptr;

// TODO: add actual support for this
void fillemptycbs()
{
    static int once;
    if (!once++)
    {
        if (CurrentTaskingMode == TaskingMode::Multi)
            return;
        string StubName = "Mono Task";
        nullpcb = new PCB;
        nullpcb->ID = 0;
        nullpcb->Parent = nullptr;
        memcpy(nullpcb->Name, StubName, sizeof(nullpcb->Name));

        nulltcb = new TCB;
        nulltcb->ID = 0;
        nulltcb->Parent = nullpcb;
        memcpy(nulltcb->Name, StubName, sizeof(nulltcb->Name));
    }
}

PCB *SysGetProcessByPID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nullpcb;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (PCB *pcb in mt->ListProcess)
        {
            if (pcb == nullptr || pcb->Checksum != Checksum::PROCESS_CHECKSUM || pcb->Elevation == ELEVATION::Idle)
                continue;
            if (pcb->ID == ID)
            {
                LeaveCriticalSection;
                return pcb;
            }
        }
        LeaveCriticalSection;
        return nullptr;
    }
    default:
        return nullptr;
    }
}

TCB *SysGetThreadByTID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nulltcb;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (PCB *pcb in mt->ListProcess)
        {
            if (pcb == nullptr || pcb->Checksum != Checksum::PROCESS_CHECKSUM || pcb->Elevation == ELEVATION::Idle)
                continue;

            foreach (TCB *tcb in pcb->Threads)
            {
                if (tcb->ID == ID)
                {
                    LeaveCriticalSection;
                    return tcb;
                }
            }
        }
        LeaveCriticalSection;
        return nullptr;
    }
    default:
        return nullptr;
    }
}

PCB *SysGetCurrentProcess()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nullpcb;
    }
    case TaskingMode::Multi:
        return CurrentCPU->CurrentProcess;
    default:
        return nullptr;
    }
}

TCB *SysGetCurrentThread()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nulltcb;
    }
    case TaskingMode::Multi:
        return CurrentCPU->CurrentThread;
    default:
        return nullptr;
    }
}

PCB *SysCreateProcess(const char *Name, ELEVATION Elevation)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nullpcb;
    }
    case TaskingMode::Multi:
        return mt->CreateProcess(SysGetCurrentProcess(), (char *)Name, Elevation);
    default:
        return nullptr;
    }
}

TCB *SysCreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t arg0, uint64_t arg1)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        static int once = 0;
        if (!once++)
            err("The current tasking mode does not support the request.");
        return nulltcb;
    }
    case TaskingMode::Multi:
        return mt->CreateThread(Parent, InstructionPointer, arg0, arg1);
    default:
        return nullptr;
    }
}

// TODO: implement for primitive tasking if enabled to suspend the current task and run the created one
PCB *SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, ELEVATION Elevation)
{
    fillemptycbs();
    /* ... Open file ... Parse file ... map elf file ... get rip etc ... */
    EnterCriticalSection;
    FILE *file = vfs->Open(File);
    if (file->Status != FILESTATUS::OK)
    {
        err("File status error %d for file %s", file->Status, File);
        goto error_exit;
    }

    if (file->Node->Flags == NodeFlags::FS_FILE)
    {
        void *FileBuffer = KernelAllocator.RequestPages(file->Node->Length / 0x1000 + 1);

        vfs->Read(file, 0, FileBuffer, file->Node->Length);
        Elf64_Ehdr *ELFHeader = (Elf64_Ehdr *)FileBuffer;

        if (ELFHeader->e_ident[EI_MAG0] == ELFMAG0 &&
            ELFHeader->e_ident[EI_MAG1] == ELFMAG1 &&
            ELFHeader->e_ident[EI_MAG2] == ELFMAG2 &&
            ELFHeader->e_ident[EI_MAG3] == ELFMAG3)
        {
            if (ELFHeader->e_ident[EI_CLASS] == ELFCLASS32)
            {
                err("32 bit ELF file not supported for now.");
                KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                goto error_exit;
            }
            if (ELFHeader->e_ident[EI_CLASS] == ELFCLASS64)
            {
                if (ELFHeader->e_type == ET_EXEC)
                {
                    debug("64 bit ELF file found.");

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

                    Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)FileBuffer) + ELFHeader->e_phoff);
                    void *addr;
                    for (int i = 0; i < ELFHeader->e_phnum; i++, pheader++)
                    {
                        if (pheader->p_type != PT_LOAD)
                            continue;
                        addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
                    }
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
                    // KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                    debug("%s Entry Point: %#llx", File, (uint64_t)(ELFHeader->e_entry + (uint64_t)offset));
                    vfs->Close(file);
                    LeaveCriticalSection;
                    if (CurrentTaskingMode == TaskingMode::Mono)
                    {
                        bool user = false;
                        if (Elevation == ELEVATION::User)
                            user = true;
                        return (/* data will be invalid but not null */ PCB *)monot->CreateTask(ELFHeader->e_entry + (uint64_t)offset, arg0, arg1, (char *)file->Name, user);
                    }
                    else
                    {
                        PCB *pcb = SysCreateProcess(file->Name, Elevation);
                        pcb->Offset = (uint64_t)offset;
                        return SysCreateThread(pcb, (uint64_t)ELFHeader->e_entry, arg0, arg1)->Parent;
                    }
                }
                else if (ELFHeader->e_type == ET_DYN)
                {
                    err("Dynamic ELF file not supported for now.");
                    KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                    goto error_exit;
                }
                else
                {
                    err("Unknown ELF file type.");
                    KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                    goto error_exit;
                }
            }
        }
        else
        {
            trace("File %s is not an elf file", File);
            IMAGE_DOS_HEADER *MZHeader = (IMAGE_DOS_HEADER *)FileBuffer;
            if (MZHeader->e_magic == IMAGE_DOS_SIGNATURE)
            {
                trace("File %s is a MZ file", File);
                IMAGE_NT_HEADERS *PEHeader = (IMAGE_NT_HEADERS *)(((char *)FileBuffer) + MZHeader->e_lfanew);
                IMAGE_OS2_HEADER *NEHeader = (IMAGE_OS2_HEADER *)(((char *)FileBuffer) + MZHeader->e_lfanew);
                if (NEHeader->ne_magic == IMAGE_OS2_SIGNATURE)
                {
                    trace("File %s is a NE file", File);
                }
                if (PEHeader->Signature == IMAGE_NT_SIGNATURE)
                {
                    trace("File %s is a PE file", File);
                    if (PEHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
                    {
                        err("32 bit PE file not supported for now.");
                        KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                        goto error_exit;
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
                            fixme("there are %ld cocks in your area and %ld bitches away from you", PEHeader->FileHeader.NumberOfSections, section->SizeOfRawData);
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
                        debug("%s Entry Point: %#llx", File, (uint64_t)(PEHeader->OptionalHeader.AddressOfEntryPoint + (uint64_t)FileBuffer));
                    }
                    else
                    {
                        err("Unknown Machine: %#lx", PEHeader->FileHeader.Machine);
                        KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                        goto error_exit;
                    }
                }
                else
                {
                    err("Unknown file type.");
                    KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
                    goto error_exit;
                }
            }
        }
    }
error_exit:
    vfs->Close(file);
    LeaveCriticalSection;
    return nullptr;
}
