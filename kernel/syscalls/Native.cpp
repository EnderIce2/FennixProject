#include "syscalls.hpp"

#include "../../libc/include/syscalls.h"
#include "../drivers/keyboard.hpp"
#include "../drivers/mouse.hpp"
#include "../drivers/serial.h"
#include "../cpu/acpi.hpp"
#include "../kernel.h"
#include "../timer.h"

#include <internal_task.h>
#include <filesystem.h>
#include <display.h>

static uint64_t internal_unimpl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g)
{
    fixme("Unimplemented Syscall: %d %d %d %d %d %d %d", a, b, c, d, e, f, g);
    return deadcall;
}

static uint64_t internal_exit(SyscallsRegs *regs, uint64_t code)
{
    trace("Userspace thread %s(%lld) exited with code %#llx", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, code);
    SysGetCurrentThread()->ExitCode = code;
    SysGetCurrentThread()->Status = Terminated;
    return 0;
}

static uint64_t internal_createprocess(SyscallsRegs *regs, char *Path, uint64_t arg0, uint64_t arg1)
{
    syscldbg("syscall: createprocess( %s %#llx %#llx )", Path, arg0, arg1);
    return SysCreateProcessFromFile(Path, arg0, arg1, User)->ID;
}

static TCB *internal_createthread(SyscallsRegs *regs, uint64_t rip, uint64_t arg0, uint64_t arg1)
{
    syscldbg("syscall: createthread( %#llx %#llx %#llx )", rip, arg0, arg1);
    return SysCreateThread(SysGetCurrentProcess(), rip, arg0, arg1);
}

static PCB *internal_getcurrentprocess(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentprocess()");
    return SysGetCurrentProcess();
}

static TCB *internal_getcurrentthread(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentthread()");
    return SysGetCurrentThread();
}

static uint64_t internal_getcurrentprocessid(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentprocessid()");
    return SysGetCurrentProcess()->ID;
}

static uint64_t internal_getcurrentthreadid(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentthreadid()");
    return SysGetCurrentThread()->ID;
}

static int internal_getschedulemode(SyscallsRegs *regs)
{
    syscldbg("syscall: getschedulemode()");
    return 0;
}

static void *internal_createtask(SyscallsRegs *regs, uint64_t rip, uint64_t arg0, uint64_t arg1, char *name)
{
    err("syscall: createtask( %#llx %#llx %#llx %s )", rip, arg0, arg1, name);
}

static void internal_pushtask(SyscallsRegs *regs, uint64_t jumpbackIP)
{
    err("syscall: pushtask( %#llx )", jumpbackIP);
}

static void internal_poptask(SyscallsRegs *regs)
{
    err("syscall: poptask()");
}

static void *internal_requestpage(SyscallsRegs *regs)
{
    syscldbg("syscall: requestpage()");
    void *ret = KernelAllocator.RequestPage();
    KernelPageTableManager.MapMemory(ret, ret, PTFlag::US | PTFlag::RW);
    return ret;
}

static void internal_freepage(SyscallsRegs *regs, void *page)
{
    syscldbg("syscall: freepage( %p )", page);
    KernelAllocator.FreePage(page);
    KernelPageTableManager.UnmapMemory(page);
    KernelPageTableManager.MapMemory(page, page, PTFlag::RW);
}

static void *internal_requestpages(SyscallsRegs *regs, uint64_t pages)
{
    syscldbg("syscall: requestpages( %#llx )", pages);
    void *ret = KernelAllocator.RequestPages(pages);
    for (uint64_t i = (uint64_t)ret; i < ((uint64_t)ret + (pages * PAGE_SIZE)); i += PAGE_SIZE)
        KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::US | PTFlag::RW);
    return ret;
}

static void internal_freepages(SyscallsRegs *regs, void *page, uint64_t pages)
{
    syscldbg("syscall: freepages( %p, %#llx )", page, pages);
    KernelAllocator.FreePages(page, pages);
    for (uint64_t i = (uint64_t)page; i < ((uint64_t)page + (pages * PAGE_SIZE)); i += PAGE_SIZE)
    {
        KernelPageTableManager.UnmapMemory((void *)i);
        KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW);
    }
}

static void internal_shutdown(SyscallsRegs *regs)
{
    warn("syscall: shutdown()");
}

static void internal_reboot(SyscallsRegs *regs)
{
    warn("syscall: reboot()");
}

static uint64_t internal_fbaddress(SyscallsRegs *regs)
{
    syscldbg("syscall: fbaddress()");
    return CurrentDisplay->GetFramebuffer()->Address;
}

static uint64_t internal_fbsize(SyscallsRegs *regs)
{
    syscldbg("syscall: fbsize()");
    return CurrentDisplay->GetFramebuffer()->Size;
}

static uint64_t internal_fbwidth(SyscallsRegs *regs)
{
    syscldbg("syscall: fbwidth()");
    return CurrentDisplay->GetFramebuffer()->Width;
}

static uint64_t internal_fbheight(SyscallsRegs *regs)
{
    syscldbg("syscall: fbheight()");
    return CurrentDisplay->GetFramebuffer()->Height;
}

static uint64_t internal_fbppsl(SyscallsRegs *regs)
{
    syscldbg("syscall: fbppsl()");
    return CurrentDisplay->GetFramebuffer()->PixelsPerScanLine;
}

static uint8_t internal_getlastkeyboardscancode(SyscallsRegs *regs)
{
    syscldbg("syscall: getlastkeyboardscancode()");
    return ps2keyboard->GetLastScanCode();
}

static File *internal_fileOpen(SyscallsRegs *regs, char *Path)
{
    err("syscall: fileOpen( %s )", Path);
    // File *f = (File *)UserAllocator->Malloc(sizeof(File));
    // FileSystem::FILE *fo = vfs->Open(Path, nullptr);

    // UserAllocator->Xstac();
    // f->Status = static_cast<FileStatus>(fo->Status);
    // memcpy(f->Name, fo->Name, sizeof(f->Name));
    // if (fo)
    // {
    //     f->Handle = fo;
    //     if (fo->Node)
    //     {
    //         f->IndexNode = fo->Node->IndexNode;
    //         f->Mask = fo->Node->Mask;
    //         f->Mode = fo->Node->Mode;
    //         f->Flags = fo->Node->Flags;
    //         f->UserIdentifier = fo->Node->UserIdentifier;
    //         f->GroupIdentifier = fo->Node->GroupIdentifier;
    //         f->Address = fo->Node->Address;
    //         f->Length = fo->Node->Length;

    //         f->Parent = fo->Node->Parent;
    //         f->Operator = fo->Node->Operator;
    //     }
    // }

    // UserAllocator->Xclac();
    return (File *)0xdeadbeef;
}

static File *internal_fileOpenWithParent(SyscallsRegs *regs, char *Path, File *Parent)
{
    err("syscall: fileOpenWithParent( %s, %p )", Path, Parent);
    // File *f = (File *)UserAllocator->Malloc(sizeof(File));
    // UserAllocator->Xstac();
    // FileSystem::FILE *fo = vfs->Open(Path, static_cast<FileSystem::FILE *>(Parent->Handle)->Node);

    // f->Status = static_cast<FileStatus>(fo->Status);
    // memcpy(f->Name, fo->Name, sizeof(f->Name));
    // if (fo)
    // {
    //     f->Handle = fo;
    //     if (fo->Node)
    //     {
    //         f->IndexNode = fo->Node->IndexNode;
    //         f->Mask = fo->Node->Mask;
    //         f->Mode = fo->Node->Mode;
    //         f->Flags = fo->Node->Flags;
    //         f->UserIdentifier = fo->Node->UserIdentifier;
    //         f->GroupIdentifier = fo->Node->GroupIdentifier;
    //         f->Address = fo->Node->Address;
    //         f->Length = fo->Node->Length;

    //         f->Parent = fo->Node->Parent;
    //         f->Operator = fo->Node->Operator;
    //     }
    // }

    // UserAllocator->Xclac();
    return (File *)0xdeadbeef;
}

static void internal_fileClose(SyscallsRegs *regs, File *F)
{
    err("syscall: fileClose( %p )", F);
    // UserAllocator->Xstac();
    // if (static_cast<FileSystem::FILE *>(F->Handle) != (void *)deadbeef)
    //     vfs->Close(static_cast<FileSystem::FILE *>(F->Handle));
    // UserAllocator->Xclac();
}

static uint64_t internal_fileRead(SyscallsRegs *regs, File *F, uint64_t Offset, void *Buffer, uint64_t Size)
{
    err("syscall: fileRead( %p, %#llx, %p, %#llx )", F, Offset, Buffer, Size);
    // UserAllocator->Xstac();
    // uint64_t ret = vfs->Read(static_cast<FileSystem::FILE *>(F->Handle), Offset, Buffer, Size);
    // UserAllocator->Xclac();
    return 0xdeadbeef;
}

static uint64_t internal_fileWrite(SyscallsRegs *regs, File *F, uint64_t Offset, void *Buffer, uint64_t Size)
{
    err("syscall: fileWrite( %p, %#llx, %p, %#llx )", F, Offset, Buffer, Size);
    // UserAllocator->Xstac();
    // uint64_t ret = vfs->Write(static_cast<FileSystem::FILE *>(F->Handle), Offset, Buffer, Size);
    // UserAllocator->Xclac();
    return 0xdeadbeef;
}

static uint64_t internal_filesize(SyscallsRegs *regs, File *File)
{
    err("syscall: filesize( %p )", File);
    // UserAllocator->Xstac();
    // uint64_t ret = static_cast<FileSystem::FILE *>(File->Handle)->Node->Length;
    // UserAllocator->Xclac();
    return 0xdeadbeef;
}

static char *internal_filefullpath(SyscallsRegs *regs, File *File)
{
    err("syscall: filefullpath( %p )", File);
    // UserAllocator->Xstac();
    // char *retmp = vfs->GetPathFromNode(static_cast<FileSystem::FILE *>(File->Handle)->Node);
    // UserAllocator->Xclac();
    // char *ret = (char *)UserAllocator->Malloc(strlen(retmp) + 1);
    // UserAllocator->Xstac();
    // strcpy(ret, retmp);
    // delete[] retmp;
    // UserAllocator->Xclac();
    return "deadbeef";
}

static uint64_t internal_filechildrensize(SyscallsRegs *regs, File *File)
{
    err("syscall: filechildrensize( %p )", File);
    // UserAllocator->Xstac();
    // uint64_t ret = static_cast<FileSystem::FILE *>(File->Handle)->Node->Children.size();
    // UserAllocator->Xclac();
    return 0xdeadbeef;
}

static File *internal_filegetchildren(SyscallsRegs *regs, File *F, uint64_t Index)
{
    err("syscall: filegetchildren( %p, %#llx )", F, Index);
    // File *f = (File *)UserAllocator->Malloc(sizeof(File));
    // UserAllocator->Xstac();
    // FileSystem::FileSystemNode *node = static_cast<FileSystem::FILE *>(F->Handle)->Node->Children[Index];

    // memcpy(f->Name, node->Name, sizeof(f->Name));
    // if (node)
    // {
    //     f->Status = FileStatus::OK;
    //     f->Handle = (void *)deadbeef;
    //     f->IndexNode = node->IndexNode;
    //     f->Mask = node->Mask;
    //     f->Mode = node->Mode;
    //     f->Flags = node->Flags;
    //     f->UserIdentifier = node->UserIdentifier;
    //     f->GroupIdentifier = node->GroupIdentifier;
    //     f->Address = node->Address;
    //     f->Length = node->Length;

    //     f->Parent = node->Parent;
    //     f->Operator = node->Operator;
    // }
    // else
    //     f->Status = FileStatus::NOT_SUPPORTED;

    // UserAllocator->Xclac();
    return (File *)0xdeadbeef;
}

static void internal_usleep(SyscallsRegs *regs, uint64_t us)
{
    syscldbg("syscall: usleep( %#llx )", us);
    warn("Sleeping in multi-tasking mode is not implemented yet!");
    return;
}

static uint64_t internal_dbg(SyscallsRegs *regs, int port, char *message)
{
    syscldbg("syscall: dbg( %d, %s )", port, message);
    serial_write_text(port, message);
    return 0;
}

static void *FennixSyscallsTable[] = {
    [_NullCall] = (void *)internal_unimpl,
    [_ProcessExit] = (void *)internal_exit,
    [_ProcessCreate] = (void *)internal_createprocess,
    [_ThreadCreate] = (void *)internal_createthread,
    [_GetCurrentProcess] = (void *)internal_getcurrentprocess,
    [_GetCurrentThread] = (void *)internal_getcurrentthread,
    [_GetCurrentProcessID] = (void *)internal_getcurrentprocessid,
    [_GetCurrentThreadID] = (void *)internal_getcurrentthreadid,

    [_GetScheduleMode] = (void *)internal_getschedulemode,

    [_CreateTask] = (void *)internal_createtask,
    [_PushTask] = (void *)internal_pushtask,
    [_PopTask] = (void *)internal_poptask,

    [_RequestPage] = (void *)internal_requestpage,
    [_FreePage] = (void *)internal_freepage,
    [_RequestPages] = (void *)internal_requestpages,
    [_FreePages] = (void *)internal_freepages,

    [_SystemInfo] = (void *)internal_unimpl,
    [_SystemTime] = (void *)internal_unimpl,
    [_SystemTimeSet] = (void *)internal_unimpl,
    [_Shutdown] = (void *)internal_shutdown,
    [_Reboot] = (void *)internal_reboot,

    [_GetFramebufferAddress] = (void *)internal_fbaddress,
    [_GetFramebufferSize] = (void *)internal_fbsize,
    [_GetFramebufferWidth] = (void *)internal_fbwidth,
    [_GetFramebufferHeight] = (void *)internal_fbheight,
    [_GetFramebufferPixelsPerScanLine] = (void *)internal_fbppsl,

    [_RegisterInterruptHandler] = (void *)internal_unimpl,
    [_UnregisterInterruptHandler] = (void *)internal_unimpl,

    [_GetLastKeyboardScanCode] = (void *)internal_getlastkeyboardscancode,

    [_FileOpen] = (void *)internal_fileOpen,
    [_FileOpenWithParent] = (void *)internal_fileOpenWithParent,
    [_FileClose] = (void *)internal_fileClose,
    [_FileRead] = (void *)internal_fileRead,
    [_FileWrite] = (void *)internal_fileWrite,
    [_FileSeek] = (void *)internal_unimpl,
    [_FileSize] = (void *)internal_filesize,
    [_FileFlush] = (void *)internal_unimpl,
    [_FileDelete] = (void *)internal_unimpl,
    [_FileRename] = (void *)internal_unimpl,
    [_FileExists] = (void *)internal_unimpl,
    [_FileCreate] = (void *)internal_unimpl,
    [_FileFullPath] = (void *)internal_filefullpath,
    [_FileChildrenSize] = (void *)internal_filechildrensize,
    [_FileGetChildren] = (void *)internal_filegetchildren,

    [_usleep] = (void *)internal_usleep,

    [_DebugMessage] = (void *)internal_dbg,
};

uint64_t HandleFennixSyscalls(SyscallsRegs *regs)
{
    if (RAX > sizeof(FennixSyscallsTable))
    {
        return internal_unimpl(regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi, regs->rbp);
    }

    uint64_t (*call)(uint64_t, ...) = reinterpret_cast<uint64_t (*)(uint64_t, ...)>(FennixSyscallsTable[RAX]);
    if (!call)
    {
        err("Syscall %#llx failed.", RAX);
        return failedcall;
    }
    syscldbg("SYSCALL->CALLDATA->(r:%lx a:%lx b:%lx c:%lx d:%lx S:%lx D:%lx)[CALLID:%d]", regs, RAX, RBX, RCX, RDX, RSI, RDI, RAX);
    uint64_t ret = call((uint64_t)regs, RBX, RDX, RSI, RDI);
    RAX = ret;
    return ret;
}
