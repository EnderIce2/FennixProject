#include <internal_task.h>

#include <filesystem.h>
#include <critical.hpp>
#include <debug.h>
#include <heap.h>
#include <elf.h>

#include "../cpu/smp.hpp"

using namespace Tasking;
using namespace FileSystem;

int CurrentTaskingMode = TaskingMode::None;

PCB *nullpcb = new PCB;
TCB *nulltcb = new TCB;

// TODO: add actual support for this
void fillemptycbs()
{
    static int once;
    if (!once++)
    {
        string StubName = "Mono Task";
        nullpcb->ID = 0;
        nullpcb->Parent = nullptr;
        memcpy(nullpcb->Name, StubName, sizeof(nullpcb->Name));

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
        Elf64_Ehdr *header = (Elf64_Ehdr *)FileBuffer;

        if (header->e_ident[EI_MAG0] != ELFMAG0)
        {
            err("ELF Header EI_MAG0 incorrect.");
            KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            goto error_exit;
        }
        if (header->e_ident[EI_MAG1] != ELFMAG1)
        {
            err("ELF Header EI_MAG1 incorrect.");
            KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            goto error_exit;
        }
        if (header->e_ident[EI_MAG2] != ELFMAG2)
        {
            err("ELF Header EI_MAG2 incorrect.");
            KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            goto error_exit;
        }
        if (header->e_ident[EI_MAG3] != ELFMAG3)
        {
            err("ELF Header EI_MAG3 incorrect.");
            KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            goto error_exit;
        }
        if (header->e_ident[EI_CLASS] == ELFCLASS32)
        {
            err("32 bit ELF file not supported for now.");
            KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            goto error_exit;
        }
        if (header->e_ident[EI_CLASS] == ELFCLASS64)
        {
            debug("64 bit ELF file found.");
            Elf64_Phdr *pheader = (Elf64_Phdr *)(((char *)FileBuffer) + header->e_phoff);
            void *addr;
            for (int i = 0; i < header->e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                addr = (void *)((uint64_t)pheader->p_vaddr + pheader->p_memsz);
            }
            void *offset = KernelAllocator.RequestPages((uint64_t)addr / 0x1000 + 1);

            pheader = (Elf64_Phdr *)(((char *)FileBuffer) + header->e_phoff);
            for (int i = 0; i < header->e_phnum; i++, pheader++)
            {
                if (pheader->p_type != PT_LOAD)
                    continue;
                void *dst = (void *)((uint64_t)pheader->p_vaddr + (uint64_t)offset);
                memset(dst, 0, pheader->p_memsz);
                memcpy(dst, ((char *)FileBuffer) + pheader->p_offset, pheader->p_filesz);
            }
            // process pages -> addr / 0x1000 + 1;
            // KernelAllocator.FreePages(FileBuffer, file->Node->Length / 0x1000 + 1);
            debug("%s Entry Point: %#llx", File, (uint64_t)(header->e_entry + (uint64_t)offset));
            vfs->Close(file);
            LeaveCriticalSection;
            if (CurrentTaskingMode == TaskingMode::Mono)
            {
                bool user = false;
                if (Elevation == ELEVATION::User)
                    user = true;
                return (/* data will be invalid but not null */ PCB *)monot->CreateTask(header->e_entry + (uint64_t)offset, arg0, arg1, (char *)file->Name, user);
            }
            else
                return SysCreateThread(SysCreateProcess(file->Name, Elevation), (uint64_t)(header->e_entry + (uint64_t)offset), arg0, arg1)->Parent;
        }
    }
error_exit:
    vfs->Close(file);
    LeaveCriticalSection;
    return nullptr;
}
