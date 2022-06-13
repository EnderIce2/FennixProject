#include <sys.h>
#include <types.h>
#include <display.h>
#include <string.h>
#include <asm.h>
#include <heap.h>
#include <internal_task.h>

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};

static const char *exception_message[] = {
    "Divide-by-zero Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception"};

// TODO: after implementing IPC, this should call the IPC handler from userspace
// TODO: exception_message it will be used in the future

void TriggerUserModeCrash(TrapFrame *regs)
{
    switch (INT_NUM)
    {
    case ISR_DivideByZero:
    {
        err("Division by zero in an user-mode thread %s(%d) on CPU %ld.", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, rdmsr(MSR_FS_BASE));
        // TODO: signal the application to stop.
        SysGetCurrentThread()->Status = Terminated;
        break;
    }
    case ISR_Debug:
    {
        break;
    }
    case ISR_NonMaskableInterrupt:
    {
        break;
    }
    case ISR_Breakpoint:
    {
        break;
    }
    case ISR_Overflow:
    {
        break;
    }
    case ISR_BoundRange:
    {
        break;
    }
    case ISR_InvalidOpcode:
    {
        err("Invalid opcode in an user-mode thread %s(%d) on CPU %ld.", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, rdmsr(MSR_FS_BASE));
        // TODO: signal the application to stop.
        SysGetCurrentThread()->Status = Terminated;
        break;
    }
    case ISR_DeviceNotAvailable:
    {
        break;
    }
    case ISR_DoubleFault:
    {
        break;
    }
    case ISR_CoprocessorSegmentOverrun:
    {
        break;
    }
    case ISR_InvalidTSS:
    {
        SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        break;
    }
    case ISR_SegmentNotPresent:
    {
        SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        break;
    }
    case ISR_StackSegmentFault:
    {
        err("Stack Segment Fault caused by an user-mode thread %s(%d) at %#lx on CPU %ld.", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, rdmsr(MSR_FS_BASE), RIP);
        // TODO: signal the application to stop.
        SysGetCurrentThread()->Status = Terminated;
        break;
    }
    case ISR_GeneralProtectionFault:
    {
        err("General Protection Fault caused by an user-mode thread %s(%d) at %#lx on CPU %ld.", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, rdmsr(MSR_FS_BASE), RIP);
        // TODO: signal the application to stop.
        SysGetCurrentThread()->Status = Terminated;
        break;
    }
    case ISR_PageFault:
    {
        err("Page fault caused by an user-mode thread %s(%d) on CPU %ld.", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, rdmsr(MSR_FS_BASE));
        uint64_t addr = readcr2().raw;
        debug("Page fault at address %#llx", addr);
        PageFaultErrorCode params = {.raw = (uint32_t)ERROR_CODE};
        err("\nPage: %s\nWrite Operation: %s\nProcessor Mode: %s\nCPU Reserved Bits: %s\nCaused By An Instruction Fetch: %s\nDescription: %s",
            params.P ? "Present" : "Not Present",
            params.W ? "Read-Only" : "Read-Write",
            params.U ? "User-Mode" : "Kernel-Mode",
            params.R ? "Reserved" : "Unreserved",
            params.I ? "Yes" : "No",
            ERROR_CODE & 0x00000008 ? "One or more page directory entries contain reserved bits which are set to 1." : pagefault_message[ERROR_CODE & 0b111]);
        // TODO: signal the application to stop.
        SysGetCurrentThread()->Status = Terminated;
        break;
    }
    case ISR_x87FloatingPoint:
    {
        break;
    }
    case ISR_AlignmentCheck:
    {
        break;
    }
    case ISR_MachineCheck:
    {
        break;
    }
    case ISR_SIMDFloatingPoint:
    {
        break;
    }
    case ISR_Virtualization:
    {
        break;
    }
    case ISR_Security:
    {
        break;
    }
    default:
    {
        break;
    }
    }
#ifdef DEBUG
    CurrentDisplay->SetPrintColor(0xFF0000);
    printf_("Usermode thread %s(%ld) crashed! Check the serial port (COM1) for more info.\n", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID);
#endif
    STI;
}
