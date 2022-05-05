#include <internal_task.h>

#include <filesystem.h>
#include <critical.hpp>
#include <debug.h>
#include <heap.h>
#include <elf.h>

using namespace MonoTasking;
using namespace MultiTasking;
using namespace FileSystem;

int CurrentTaskingMode = TaskingMode::None;

ProcessControlBlock *APICALL SysGetProcessByPID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ProcessControlBlock *proc = nullptr;
        fixme("unimplemented for primitive tasking");
        return proc;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (auto var in MultiProcessing->GetVectorProcessList())
        {
            if (var->ProcessID == ID)
            {
                LeaveCriticalSection;
                return var;
            }
        }
        LeaveCriticalSection;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ThreadControlBlock *APICALL SysGetThreadByTID(uint64_t ID)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        err("not supported");
        return nullptr;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        foreach (auto var in MultiProcessing->GetVectorProcessList())
        {
            foreach (auto var in var->Threads)
            {
                if (var->ThreadID == ID)
                {
                    LeaveCriticalSection;
                    return var;
                }
            }
        }
        LeaveCriticalSection;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ProcessControlBlock *APICALL SysGetCurrentProcess()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ProcessControlBlock *proc = nullptr;
        fixme("unimplemented for primitive tasking");
        return proc;
    }
    case TaskingMode::Multi:
    {
        return MultiProcessing->GetCurrentProcess();
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ThreadControlBlock *APICALL SysGetCurrentThread()
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ThreadControlBlock *proc = nullptr;
        fixme("unimplemented for primitive tasking");
        return proc;
    }
    case TaskingMode::Multi:
    {
        return MultiProcessing->GetCurrentThread();
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ProcessControlBlock *APICALL SysCreateProcess(const char *Name, void *PageTable)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ProcessControlBlock *proc = new ProcessControlBlock;
        memcpy(proc->Name, Name, sizeof(proc->Name));
        if (PageTable != nullptr)
            proc->PageTable = PageTable;
        else
            proc->PageTable = KernelPageTableAllocator->CreatePageTable();
        return proc;
    }
    case TaskingMode::Multi:
    {
        EnterCriticalSection;
        ProcessControlBlock *proc = MultiProcessing->CreateProcess(SysGetCurrentProcess(), (char *)Name);
        if (PageTable != nullptr)
        {
            KernelAllocator.FreePage(proc->PageTable);
            proc->PageTable = (VMM::PageTable *)PageTable;
        }
        LeaveCriticalSection;
        return proc;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

ThreadControlBlock *APICALL SysCreateThread(ProcessControlBlock *Parent, uint64_t InstructionPointer, uint64_t arg0, uint64_t arg1, bool UserMode)
{
    switch (CurrentTaskingMode)
    {
    case TaskingMode::Mono:
    {
        ThreadControlBlock *thread = new ThreadControlBlock;
        TaskControlBlock *task = SingleProcessing->CreateTask(InstructionPointer, 0, 0, Parent->Name, UserMode);
        Parent->ProcessID = task->id;
        if (!Parent->PageTable)
            Parent->PageTable = (VMM::PageTable *)task->pml4;
        thread->Checksum = THREAD_CHECKSUM;
        thread->ExitCode = 0;
        thread->Parent = Parent;
        thread->Policy = POLICY_KERNEL;
        thread->Priority = PRIORITY_MEDIUM;
        thread->State = STATE_READY;
        thread->Registers = task->regs;
        thread->Stack = task->stack;
        thread->ThreadID = task->id;
        Parent->Threads[0] = thread;
        return thread;
    }
    case TaskingMode::Multi:
    {
        ThreadControlBlock *thread = MultiProcessing->CreateThread(Parent, InstructionPointer, arg0, arg1, ControlBlockPriority::PRIORITY_MEDIUM,
                                                                   ControlBlockState::STATE_READY, ControlBlockPolicy::POLICY_KERNEL, UserMode);
        return thread;
    }
    case TaskingMode::None:
        return nullptr;
    default:
        return nullptr;
    }
    return nullptr;
}

// TODO: implement for primitive tasking if enabled to suspend the current task and run the created one
ProcessControlBlock *APICALL SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, bool UserMode)
{
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
            return SysCreateThread(SysCreateProcess(file->Name, nullptr), (uint64_t)(header->e_entry + (uint64_t)offset), arg0, arg1, UserMode)->Parent;
        }
    }
error_exit:
    vfs->Close(file);
    LeaveCriticalSection;
    return nullptr;
}
