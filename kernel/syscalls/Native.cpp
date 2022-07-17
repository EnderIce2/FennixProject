#include "syscalls.hpp"

#include "../../libc/include/syscalls.h"
#include "../security/security.hpp"
#include "../drivers/keyboard.hpp"
#include "../drivers/mouse.hpp"
#include "../drivers/serial.h"
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
    if (CurrentTaskingMode == TaskingMode::Mono)
    {
        Tasking::monot->KillMe();
        return 0;
    }
    trace("Userspace thread %s(%lld) exited with code %#llx", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, code);
    SysGetCurrentThread()->ExitCode = code;
    SysGetCurrentThread()->Status = Terminated;
    return 0;
}

static uint64_t internal_createprocess(SyscallsRegs *regs, char *Path, uint64_t arg0, uint64_t arg1)
{
    syscldbg("syscall: createprocess( %s %#llx %#llx )", Path, arg0, arg1);
    if (!CanSyscall(regs))
        return deniedcall;
    return SysCreateProcessFromFile(Path, arg0, arg1, User)->ID;
}

static TCB *internal_createthread(SyscallsRegs *regs, uint64_t rip, uint64_t arg0, uint64_t arg1)
{
    syscldbg("syscall: createthread( %#llx %#llx %#llx )", rip, arg0, arg1);
    if (!CanSyscall(regs))
        return (TCB *)deniedcall;
    return SysCreateThread(SysGetCurrentProcess(), rip, arg0, arg1);
}

static PCB *internal_getcurrentprocess(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentprocess()");
    if (!CanSyscall(regs))
        return (PCB *)deniedcall;
    return SysGetCurrentProcess();
}

static TCB *internal_getcurrentthread(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentthread()");
    if (!CanSyscall(regs))
        return (TCB *)deniedcall;
    return SysGetCurrentThread();
}

static uint64_t internal_getcurrentprocessid(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentprocessid()");
    if (!CanSyscall(regs))
        return deniedcall;
    return SysGetCurrentProcess()->ID;
}

static uint64_t internal_getcurrentthreadid(SyscallsRegs *regs)
{
    syscldbg("syscall: getcurrentthreadid()");
    if (!CanSyscall(regs))
        return deniedcall;
    return SysGetCurrentThread()->ID;
}

static int internal_getschedulemode(SyscallsRegs *regs)
{
    syscldbg("syscall: getschedulemode()");
    return CurrentTaskingMode;
}

static Tasking::TaskControlBlock *internal_createtask(SyscallsRegs *regs, uint64_t rip, uint64_t arg0, uint64_t arg1, char *name)
{
    syscldbg("syscall: createtask( %#llx %#llx %#llx %s )", rip, arg0, arg1, name);
    if (!CanSyscall(regs))
        return (Tasking::TaskControlBlock *)deniedcall;
    return Tasking::monot->CreateTask(rip, arg0, arg1, name, true);
}

static void internal_pushtask(SyscallsRegs *regs, uint64_t a, uint64_t b, uint64_t c, uint64_t d)
{
    syscldbg("syscall: pushtask( %#llx %#llx %#llx %#llx )", a, b, c, d);
    if (!CanSyscall(regs))
        return;
    Tasking::monot->PushTask(RIP);
}

static void internal_poptask(SyscallsRegs *regs)
{
    syscldbg("syscall: poptask()");
    if (!CanSyscall(regs))
        return;
    Tasking::monot->PopTask();
}

static void *internal_requestpage(SyscallsRegs *regs)
{
    syscldbg("syscall: requestpage()");
    if (!CanSyscall(regs))
        return (void *)deniedcall;
    void *ret = KernelAllocator.RequestPage();
    KernelPageTableManager.MapMemory(ret, ret, PTFlag::US | PTFlag::RW);
    return ret;
}

static void internal_freepage(SyscallsRegs *regs, void *page)
{
    syscldbg("syscall: freepage( %p )", page);
    if (!CanSyscall(regs))
        return;
    KernelAllocator.FreePage(page);
    KernelPageTableManager.UnmapMemory(page);
    KernelPageTableManager.MapMemory(page, page, PTFlag::RW);
}

static void *internal_requestpages(SyscallsRegs *regs, uint64_t pages)
{
    syscldbg("syscall: requestpages( %#llx )", pages);
    if (!CanSyscall(regs))
        return (void *)deniedcall;
    void *ret = KernelAllocator.RequestPages(pages);
    for (uint64_t i = (uint64_t)ret; i < ((uint64_t)ret + (pages * PAGE_SIZE)); i += PAGE_SIZE)
        KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::US | PTFlag::RW);
    return ret;
}

static void internal_freepages(SyscallsRegs *regs, void *page, uint64_t pages)
{
    syscldbg("syscall: freepages( %p, %#llx )", page, pages);
    if (!CanSyscall(regs))
        return;
    KernelAllocator.FreePages(page, pages);
    for (uint64_t i = (uint64_t)page; i < ((uint64_t)page + (pages * PAGE_SIZE)); i += PAGE_SIZE)
    {
        KernelPageTableManager.UnmapMemory((void *)i);
        KernelPageTableManager.MapMemory((void *)i, (void *)i, PTFlag::RW);
    }
}

static uint64_t internal_fbaddress(SyscallsRegs *regs)
{
    syscldbg("syscall: fbaddress()");
    if (!CanSyscall(regs))
        return deniedcall;
    return CurrentDisplay->GetFramebuffer()->Address;
}

static uint64_t internal_fbsize(SyscallsRegs *regs)
{
    syscldbg("syscall: fbsize()");
    if (!CanSyscall(regs))
        return deniedcall;
    return CurrentDisplay->GetFramebuffer()->Size;
}

static uint64_t internal_fbwidth(SyscallsRegs *regs)
{
    syscldbg("syscall: fbwidth()");
    if (!CanSyscall(regs))
        return deniedcall;
    return CurrentDisplay->GetFramebuffer()->Width;
}

static uint64_t internal_fbheight(SyscallsRegs *regs)
{
    syscldbg("syscall: fbheight()");
    if (!CanSyscall(regs))
        return deniedcall;
    return CurrentDisplay->GetFramebuffer()->Height;
}

static uint64_t internal_fbppsl(SyscallsRegs *regs)
{
    syscldbg("syscall: fbppsl()");
    if (!CanSyscall(regs))
        return deniedcall;
    return CurrentDisplay->GetFramebuffer()->PixelsPerScanLine;
}

static uint8_t internal_getlastkeyboardscancode(SyscallsRegs *regs)
{
    syscldbg("syscall: getlastkeyboardscancode()");
    return ps2keyboard->GetLastScanCode();
}

static File *internal_fileOpen(SyscallsRegs *regs, char *Path)
{
    syscldbg("syscall: fileOpen( %s )", Path);
    if (!CanSyscall(regs))
        return (File *)deniedcall;

    File *f = (File *)UserAllocator->Malloc(sizeof(File));
    FileSystem::FILE *fo = vfs->Open(Path, nullptr);

    UserAllocator->Xstac();
    f->Status = static_cast<FileStatus>(fo->Status);
    memcpy(f->Name, fo->Name, sizeof(f->Name));
    if (fo)
    {
        f->Handle = fo;
        f->IndexNode = fo->Node->IndexNode;
        f->Mask = fo->Node->Mask;
        f->Mode = fo->Node->Mode;
        f->Flags = fo->Node->Flags;
        f->UserIdentifier = fo->Node->UserIdentifier;
        f->GroupIdentifier = fo->Node->GroupIdentifier;
        f->Address = fo->Node->Address;
        f->Length = fo->Node->Length;
        f->Parent = fo->Node->Parent;
        f->Operator = fo->Node->Operator;
    }

    UserAllocator->Xclac();
    return f;
}

static void internal_fileClose(SyscallsRegs *regs, File *F)
{
    syscldbg("syscall: fileClose( %p )", F);
    if (!CanSyscall(regs))
        return;
    UserAllocator->Xstac();
    vfs->Close((FileSystem::FILE *)F->Handle);
    UserAllocator->Xclac();
}

static uint64_t internal_fileRead(SyscallsRegs *regs, File *F, uint64_t Offset, void *Buffer, uint64_t Size)
{
    syscldbg("syscall: fileRead( %p, %#llx, %p, %#llx )", F, Offset, Buffer, Size);
    if (!CanSyscall(regs))
        return deniedcall;
    UserAllocator->Xstac();
    uint64_t ret = vfs->Read((FileSystem::FILE *)F->Handle, Offset, Buffer, Size);
    UserAllocator->Xclac();
    return ret;
}

static uint64_t internal_fileWrite(SyscallsRegs *regs, File *F, uint64_t Offset, void *Buffer, uint64_t Size)
{
    syscldbg("syscall: fileWrite( %p, %#llx, %p, %#llx )", F, Offset, Buffer, Size);
    if (!CanSyscall(regs))
        return deniedcall;
    UserAllocator->Xstac();
    uint64_t ret = vfs->Write((FileSystem::FILE *)F->Handle, Offset, Buffer, Size);
    UserAllocator->Xclac();
    return ret;
}

static uint64_t internal_filesize(SyscallsRegs *regs, File *File)
{
    syscldbg("syscall: filesize( %p )", File);
    if (!CanSyscall(regs))
        return deniedcall;
    UserAllocator->Xstac();
    uint64_t ret = ((FileSystem::FILE *)File->Handle)->Node->Length;
    UserAllocator->Xclac();
    return ret;
}

static void internal_usleep(SyscallsRegs *regs, uint64_t us)
{
    syscldbg("syscall: usleep( %#llx )", us);
    if (CurrentTaskingMode == TaskingMode::Mono)
    {
        usleep(us);
        return;
    }
    else if (CurrentTaskingMode == TaskingMode::Multi)
    {
        warn("Sleeping in multi-tasking mode is not implemented yet!");
        return;
    }
}

static uint64_t internal_dbg(SyscallsRegs *regs, int port, char *message)
{
    syscldbg("syscall: dbg( %d, %s )", port, message);
    if (!CanSyscall(regs))
        return deniedcall;
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

    [_GetFramebufferAddress] = (void *)internal_fbaddress,
    [_GetFramebufferSize] = (void *)internal_fbsize,
    [_GetFramebufferWidth] = (void *)internal_fbwidth,
    [_GetFramebufferHeight] = (void *)internal_fbheight,
    [_GetFramebufferPixelsPerScanLine] = (void *)internal_fbppsl,

    [_RegisterInterruptHandler] = (void *)internal_unimpl,
    [_UnregisterInterruptHandler] = (void *)internal_unimpl,

    [_GetLastKeyboardScanCode] = (void *)internal_getlastkeyboardscancode,

    [_FileOpen] = (void *)internal_fileOpen,
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
