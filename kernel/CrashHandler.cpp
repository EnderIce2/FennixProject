#include <internal_task.h>
#include <symbols.hpp>
#include <display.h>
#include <string.h>
#include <types.h>
#include <heap.h>
#include <sys.h>
#include <asm.h>

#include "cpu/smp.hpp"

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};

#define SET_PRINT_MID(text, y)                                                                                                                              \
    CurrentDisplay->SetPrintLocation((CurrentDisplay->GetFramebuffer()->Width - (strlen(text) * CurrentDisplay->CurrentFont->GetFontSize().Width)) / 2, y); \
    printf(text)
#define SET_PRINT_RIGHT(text, y)                                                                                                                      \
    CurrentDisplay->SetPrintLocation(CurrentDisplay->GetFramebuffer()->Width - (strlen(text) * CurrentDisplay->CurrentFont->GetFontSize().Width), y); \
    printf(text)

#define SET_MID(text, y) CurrentDisplay->SetPrintLocation((CurrentDisplay->GetFramebuffer()->Width - (strlen(text) * CurrentDisplay->CurrentFont->GetFontSize().Width)) / 2, y)
#define SET_RIGHT(text, y) CurrentDisplay->SetPrintLocation(CurrentDisplay->GetFramebuffer()->Width - (strlen(text) * CurrentDisplay->CurrentFont->GetFontSize().Width), y)

#define FHeight(x) ((CurrentDisplay->GetFramebuffer()->Height / 2) - (CurrentDisplay->CurrentFont->GetFontSize().Height * x))

#define staticbuffer(name) char name[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

EXTERNC void crash(string message, bool clear)
{
    // TODO: Add more useful information.
    CLI;
    debug("System crashed with message: %s", message);
    if (CurrentDisplay == nullptr)
    {
        debug("No display found. Cannot display crash message.");
        CPU_HALT;
    }
    if (clear)
        CurrentDisplay->Clear(0x121160);

    CurrentDisplay->SetPrintColor(0xDD2920);
    SET_PRINT_MID((char *)"System crashed!", FHeight(1));
    CurrentDisplay->ResetPrintColor();
    SET_PRINT_MID((char *)message, (CurrentDisplay->GetFramebuffer()->Height / 2));
    if (CurrentCPU->CurrentThread != nullptr)
    {
        err("\"%s\" happened while running thread %s(%d)", message, CurrentCPU->CurrentThread->Name, CurrentCPU->CurrentThread->ID);
    }
    CPU_HALT;
}

EXTERNC __attribute__((no_stack_protector)) void isrcrash(TrapFrame *regs)
{
    CLI;
    debug("Reading control registers...");
    CR0 cr0 = readcr0();
    CR2 cr2 = readcr2();
    CR3 cr3 = readcr3();
    CR4 cr4 = readcr4();
    CR8 cr8 = readcr8();
    EFER efer;
    efer.raw = rdmsr(MSR_EFER);

    if ((uint64_t)KernelPageTableManager.PML4 != cr3.raw)
    {
        err("Page table still not set to kernel one!");
        CR3 tmpcr3;
        tmpcr3.raw = (uint64_t)KernelPageTableManager.PML4;
        writecr3(tmpcr3);

        CR3 Result = readcr3();
        if (Result.raw != (uint64_t)KernelPageTableManager.PML4)
            err("Kernel page table failed to be set.");
    }

#if defined(__amd64__)
    debug("Reading debug registers...");
    uint64_t dr0, dr1, dr2, dr3, dr6;
    DR7 dr7;

    // store debug registers
    asm volatile("movq %%dr0, %0"
                 : "=r"(dr0));
    asm volatile("movq %%dr1, %0"
                 : "=r"(dr1));
    asm volatile("movq %%dr2, %0"
                 : "=r"(dr2));
    asm volatile("movq %%dr3, %0"
                 : "=r"(dr3));
    asm volatile("movq %%dr6, %0"
                 : "=r"(dr6));
    asm volatile("movq %%dr7, %0"
                 : "=r"(dr7));
#endif

    if (CS != 0x1b)
        CurrentDisplay->Clear(0x000000);

    switch (INT_NUM)
    {
    case ISR_DivideByZero:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_Debug:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            CurrentDisplay->SetPrintColor(0xDD2920);
            SET_PRINT_MID((char *)"System crashed!", FHeight(2));
            CurrentDisplay->ResetPrintColor();
            SET_PRINT_MID((char *)"Kernel triggered debug exception.", FHeight(1));
        }
        break;
    }
    case ISR_NonMaskableInterrupt:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_Breakpoint:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_Overflow:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_BoundRange:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_InvalidOpcode:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            CurrentDisplay->SetPrintColor(0xDD2920);
            SET_PRINT_MID((char *)"System crashed!", FHeight(2));
            CurrentDisplay->ResetPrintColor();
            SET_PRINT_MID((char *)"Kernel tried to execute an invalid opcode.", FHeight(1));
        }
        break;
    }
    case ISR_DeviceNotAvailable:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_DoubleFault:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_CoprocessorSegmentOverrun:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_InvalidTSS:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        }
        break;
    }
    case ISR_SegmentNotPresent:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        }
        break;
    }
    case ISR_StackSegmentFault:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            staticbuffer(descbuf);
            staticbuffer(desc_ext);
            staticbuffer(desc_table);
            staticbuffer(desc_idx);
            staticbuffer(desc_tmp);
            SelectorErrorCode SelCode = {.raw = ERROR_CODE};
            switch (SelCode.Table)
            {
            case 0b00:
                memcpy(desc_tmp, "GDT", 3);
                break;
            case 0b01:
                memcpy(desc_tmp, "IDT", 3);
                break;
            case 0b10:
                memcpy(desc_tmp, "LDT", 3);
                break;
            case 0b11:
                memcpy(desc_tmp, "IDT", 3);
                break;
            default:
                memcpy(desc_tmp, "Unknown", 7);
                break;
            }
            debug("external:%d table:%d idx:%#x", SelCode.External, SelCode.Table, SelCode.Idx);
            sprintf_(descbuf, "Stack segment fault at address %#lx", RIP);
            SET_PRINT_MID((char *)descbuf, FHeight(5));
            sprintf_(desc_ext, "External: %d", SelCode.External);
            SET_PRINT_MID((char *)desc_ext, FHeight(3));
            sprintf_(desc_table, "Table: %d (%s)", SelCode.Table, desc_tmp);
            SET_PRINT_MID((char *)desc_table, FHeight(2));
            sprintf_(desc_idx, "%s Index: %#x", desc_tmp, SelCode.Idx);
            SET_PRINT_MID((char *)desc_idx, FHeight(1));
            CurrentDisplay->SetPrintColor(0xDD2920);
            SET_PRINT_MID((char *)"System crashed!", FHeight(6));
            CurrentDisplay->ResetPrintColor();
            SET_PRINT_MID((char *)"More info about the exception:", FHeight(4));
            break;
        }
    }
    case ISR_GeneralProtectionFault:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
        }
        else
        {
            staticbuffer(descbuf);
            staticbuffer(desc_ext);
            staticbuffer(desc_table);
            staticbuffer(desc_idx);
            staticbuffer(desc_tmp);
            SelectorErrorCode SelCode = {.raw = ERROR_CODE};
            switch (SelCode.Table)
            {
            case 0b00:
                memcpy(desc_tmp, "GDT", 3);
                break;
            case 0b01:
                memcpy(desc_tmp, "IDT", 3);
                break;
            case 0b10:
                memcpy(desc_tmp, "LDT", 3);
                break;
            case 0b11:
                memcpy(desc_tmp, "IDT", 3);
                break;
            default:
                memcpy(desc_tmp, "Unknown", 7);
                break;
            }
            debug("external:%d table:%d idx:%#x", SelCode.External, SelCode.Table, SelCode.Idx);
            CurrentDisplay->SetPrintColor(0xDD2920);
            SET_PRINT_MID((char *)"System crashed!", FHeight(6));
            CurrentDisplay->ResetPrintColor();
            SET_PRINT_MID((char *)"More info about the exception:", FHeight(4));
            sprintf_(descbuf, "Kernel performed an illegal operation at address %#lx", RIP);
            SET_PRINT_MID((char *)descbuf, FHeight(5));
            sprintf_(desc_ext, "External: %d", SelCode.External);
            SET_PRINT_MID((char *)desc_ext, FHeight(3));
            sprintf_(desc_table, "Table: %d (%s)", SelCode.Table, desc_tmp);
            SET_PRINT_MID((char *)desc_table, FHeight(2));
            sprintf_(desc_idx, "%s Index: %#x", desc_tmp, SelCode.Idx);
            SET_PRINT_MID((char *)desc_idx, FHeight(1));
        }
        break;
    }
    case ISR_PageFault:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
            err("Kernel Exception");
            PageFaultErrorCode params = {.raw = (uint32_t)ERROR_CODE};

            // We can't use an allocator in exceptions (because that can cause another exception!) so, we'll just use a static buffer.
            staticbuffer(ret_err);
            staticbuffer(page_present);
            staticbuffer(page_write);
            staticbuffer(page_user);
            staticbuffer(page_reserved);
            staticbuffer(page_fetch);
            staticbuffer(page_protection);
            staticbuffer(page_shadow);
            staticbuffer(page_sgx);

            CurrentDisplay->SetPrintColor(0xDD2920);
            SET_PRINT_MID((char *)"System crashed!", FHeight(12));
            CurrentDisplay->ResetPrintColor();
            sprintf_(ret_err, "An exception occurred at %#lx by %#lx", cr2.PFLA, RIP);
            SET_PRINT_MID((char *)ret_err, FHeight(11));
            sprintf_(page_present, "Page: %s", params.P ? "Present" : "Not Present");
            SET_PRINT_MID((char *)page_present, FHeight(10));
            sprintf_(page_write, "Write Operation: %s", params.W ? "Read-Only" : "Read-Write");
            SET_PRINT_MID((char *)page_write, FHeight(9));
            sprintf_(page_user, "Processor Mode: %s", params.U ? "User-Mode" : "Kernel-Mode");
            SET_PRINT_MID((char *)page_user, FHeight(8));
            sprintf_(page_reserved, "CPU Reserved Bits: %s", params.R ? "Reserved" : "Unreserved");
            SET_PRINT_MID((char *)page_reserved, FHeight(7));
            sprintf_(page_fetch, "Caused By An Instruction Fetch: %s", params.I ? "Yes" : "No");
            SET_PRINT_MID((char *)page_fetch, FHeight(6));
            sprintf_(page_protection, "Caused By A Protection-Key Violation: %s", params.PK ? "Yes" : "No");
            SET_PRINT_MID((char *)page_protection, FHeight(5));
            sprintf_(page_shadow, "Caused By A Shadow Stack Access: %s", params.SS ? "Yes" : "No");
            SET_PRINT_MID((char *)page_shadow, FHeight(4));
            sprintf_(page_sgx, "Caused By An SGX Violation: %s", params.SGX ? "Yes" : "No");
            SET_PRINT_MID((char *)page_sgx, FHeight(3));
            if (ERROR_CODE & 0x00000008)
            {
                SET_PRINT_MID((char *)"One or more page directory entries contain reserved bits which are set to 1.", FHeight(2));
            }
            else
            {
                SET_PRINT_MID((char *)pagefault_message[ERROR_CODE & 0b111], FHeight(2));
            }
            err("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s", page_present, page_write, page_user, page_reserved, page_fetch, page_protection, page_shadow, page_sgx);
        }
        break;
    }
    case ISR_x87FloatingPoint:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_AlignmentCheck:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_MachineCheck:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_SIMDFloatingPoint:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_Virtualization:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    case ISR_Security:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    default:
    {
        if (CS == 0x1b)
        {
            TriggerUserModeCrash(regs);
            return;
        }
        else
        {
        }
        break;
    }
    }
    CurrentDisplay->ResetPrintPosition();
    CurrentDisplay->SetPrintColor(0x7981FC);
    printf("Technical Informations on CPU %ld:\n", rdmsr(MSR_FS_BASE));
    printf("FS=%#lx  GS=%#lx  SS=%#lx  CS=%#lx\n", rdmsr(MSR_FS_BASE), rdmsr(MSR_GS_BASE), _SS, CS);
    printf("R8=%#lx  R9=%#lx  R10=%#lx  R11=%#lx\n", R8, R9, R10, R11);
    printf("R12=%#lx  R13=%#lx  R14=%#lx  R15=%#lx\n", R12, R13, R14, R15);
    printf("RAX=%#lx  RBX=%#lx  RCX=%#lx  RDX=%#lx\n", RAX, RBX, RCX, RDX);
    printf("RSI=%#lx  RDI=%#lx  RBP=%#lx  RSP=%#lx\n", RSI, RDI, RBP, RSP);
    printf("RIP=%#lx  RFL=%#lx  INT=%#lx  ERR=%#lx  EFER=%#lx\n", RIP, FLAGS.raw, INT_NUM, ERROR_CODE, efer.raw);
    printf("CR0=%#lx  CR2=%#lx  CR3=%#lx  CR4=%#lx  CR8=%#lx\n", cr0.raw, cr2.raw, cr3.raw, cr4.raw, cr8.raw);
#if defined(__amd64__)
    printf("DR0=%#lx  DR1=%#lx  DR2=%#lx  DR3=%#lx  DR6=%#lx  DR7=%#lx\n", dr0, dr1, dr2, dr3, dr6, dr7.raw);
#endif

    CurrentDisplay->SetPrintColor(0xFC797B);
    printf("CR0: PE:%s     MP:%s     EM:%s     TS:%s\n     ET:%s     NE:%s     WP:%s     AM:%s\n     NW:%s     CD:%s     PG:%s\n     R0:%#x R1:%#x R2:%#x\n",
           cr0.PE ? "True " : "False", cr0.MP ? "True " : "False", cr0.EM ? "True " : "False", cr0.TS ? "True " : "False",
           cr0.ET ? "True " : "False", cr0.NE ? "True " : "False", cr0.WP ? "True " : "False", cr0.AM ? "True " : "False",
           cr0.NW ? "True " : "False", cr0.CD ? "True " : "False", cr0.PG ? "True " : "False",
           cr0._reserved0, cr0._reserved1, cr0._reserved2);

    CurrentDisplay->SetPrintColor(0xFCBD79);
    printf("CR2: PFLA: %#lx\n",
           cr2.PFLA);

    CurrentDisplay->SetPrintColor(0x79FC84);
    printf("CR3: PWT:%s     PCD:%s    PDBR:%#lx\n",
           cr3.PWT ? "True " : "False", cr3.PCD ? "True " : "False", cr3.PDBR);

    CurrentDisplay->SetPrintColor(0xBD79FC);
    printf("CR4: VME:%s     PVI:%s     TSD:%s      DE:%s\n     PSE:%s     PAE:%s     MCE:%s     PGE:%s\n     PCE:%s    UMIP:%s  OSFXSR:%s OSXMMEXCPT:%s\n    LA57:%s    VMXE:%s    SMXE:%s   PCIDE:%s\n OSXSAVE:%s    SMEP:%s    SMAP:%s     PKE:%s\n     R0:%#x R1:%#x R2:%#x\n",
           cr4.VME ? "True " : "False", cr4.PVI ? "True " : "False", cr4.TSD ? "True " : "False", cr4.DE ? "True " : "False",
           cr4.PSE ? "True " : "False", cr4.PAE ? "True " : "False", cr4.MCE ? "True " : "False", cr4.PGE ? "True " : "False",
           cr4.PCE ? "True " : "False", cr4.UMIP ? "True " : "False", cr4.OSFXSR ? "True " : "False", cr4.OSXMMEXCPT ? "True " : "False",
           cr4.LA57 ? "True " : "False", cr4.VMXE ? "True " : "False", cr4.SMXE ? "True " : "False", cr4.PCIDE ? "True " : "False",
           cr4.OSXSAVE ? "True " : "False", cr4.SMEP ? "True " : "False", cr4.SMAP ? "True " : "False", cr4.PKE ? "True " : "False",
           cr4._reserved0, cr4._reserved1, cr4._reserved2);

    CurrentDisplay->SetPrintColor(0x79FCF5);
    printf("CR8: TPL:%d\n", cr8.TPL);

    CurrentDisplay->SetPrintColor(0xFCFC02);
    printf("RFL: CF:%s     PF:%s     AF:%s     ZF:%s\n     SF:%s     TF:%s     IF:%s     DF:%s\n     OF:%s   IOPL:%s     NT:%s     RF:%s\n     VM:%s     AC:%s    VIF:%s    VIP:%s\n     ID:%s     AlwaysOne:%d\n     R0:%#x R1:%#x R2:%#x R3:%#x\n",
           FLAGS.CF ? "True " : "False", FLAGS.PF ? "True " : "False", FLAGS.AF ? "True " : "False", FLAGS.ZF ? "True " : "False",
           FLAGS.SF ? "True " : "False", FLAGS.TF ? "True " : "False", FLAGS.IF ? "True " : "False", FLAGS.DF ? "True " : "False",
           FLAGS.OF ? "True " : "False", FLAGS.IOPL ? "True " : "False", FLAGS.NT ? "True " : "False", FLAGS.RF ? "True " : "False",
           FLAGS.VM ? "True " : "False", FLAGS.AC ? "True " : "False", FLAGS.VIF ? "True " : "False", FLAGS.VIP ? "True " : "False",
           FLAGS.ID ? "True " : "False", FLAGS.always_one,
           FLAGS._reserved0, FLAGS._reserved1, FLAGS._reserved2, FLAGS._reserved3);

#if defined(__amd64__)
    CurrentDisplay->SetPrintColor(0xA0F0F0);
    printf("DR7: LDR0:%s     GDR0:%s     LDR1:%s     GDR1:%s\n     LDR2:%s     GDR2:%s     LDR3:%s     GDR3:%s\n     CDR0:%s     SDR0:%s     CDR1:%s     SDR1:%s\n     CDR2:%s     SDR2:%s     CDR3:%s     SDR3:%s\n     R:%#x\n",
           dr7.LocalDR0 ? "True " : "False", dr7.GlobalDR0 ? "True " : "False", dr7.LocalDR1 ? "True " : "False", dr7.GlobalDR1 ? "True " : "False",
           dr7.LocalDR2 ? "True " : "False", dr7.GlobalDR2 ? "True " : "False", dr7.LocalDR3 ? "True " : "False", dr7.GlobalDR3 ? "True " : "False",
           dr7.ConditionsDR0 ? "True " : "False", dr7.SizeDR0 ? "True " : "False", dr7.ConditionsDR1 ? "True " : "False", dr7.SizeDR1 ? "True " : "False",
           dr7.ConditionsDR2 ? "True " : "False", dr7.SizeDR2 ? "True " : "False", dr7.ConditionsDR3 ? "True " : "False", dr7.SizeDR3 ? "True " : "False",
           dr7.Reserved);
#endif

    CurrentDisplay->SetPrintColor(0x009FF0);
    printf("EFER: SCE:%s      LME:%s      LMA:%s      NXE:%s\n     SVME:%s    LMSLE:%s    FFXSR:%s      TCE:%s\n     R0:%#x R1:%#x R2:%#x\n",
           efer.SCE ? "True " : "False", efer.LME ? "True " : "False", efer.LMA ? "True " : "False", efer.NXE ? "True " : "False",
           efer.SVME ? "True " : "False", efer.LMSLE ? "True " : "False", efer.FFXSR ? "True " : "False", efer.TCE ? "True " : "False",
           efer.Reserved0, efer.Reserved1, efer.Reserved2);

// restore debug registers
#if defined(__amd64__)
    debug("Restoring debug registers...");
    asm volatile("movq %0, %%dr0"
                 :
                 : "r"(dr0));
    asm volatile("movq %0, %%dr1"
                 :
                 : "r"(dr1));
    asm volatile("movq %0, %%dr2"
                 :
                 : "r"(dr2));
    asm volatile("movq %0, %%dr3"
                 :
                 : "r"(dr3));
    asm volatile("movq %0, %%dr6"
                 :
                 : "r"(dr6));
    asm volatile("movq %0, %%dr7"
                 :
                 : "r"(dr7));
#endif

    struct StackFrame
    {
        struct StackFrame *rbp;
        uint64_t rip;
    };

    struct StackFrame *frames = (struct StackFrame *)RBP; // (struct StackFrame *)__builtin_frame_address(0);

    debug("Stack tracing...");

    CurrentDisplay->SetPrintColor(0x7981FC);
    printf("\nStack Trace:\n");

    if (!frames || !frames->rip || !frames->rbp)
    {
        CurrentDisplay->SetPrintColor(0xFF0000);
        printf("\n< No stack trace available. >\n");
        CPU_HALT;
    }
    else
    {
        frames = (struct StackFrame *)RBP;
        if (RIP >= 0xFFFFFFFF80000000 && RIP <= (uint64_t)&_kernel_end)
            debug("%p-%s <- Exception", (void *)RIP, SymTbl->GetSymbolFromAddress(RIP));
        else
            debug("%p-OUTSIDE KERNEL <- Exception", (void *)RIP);
        for (uint64_t frame = 0; frame < 100; ++frame)
        {
            if (!frames->rip)
                break;
            if (frames->rip >= 0xFFFFFFFF80000000 && frames->rip <= (uint64_t)&_kernel_end)
                debug("%p-%s", (void *)frames->rip, SymTbl->GetSymbolFromAddress(frames->rip));
            else
                debug("%p-OUTSIDE KERNEL", (void *)frames->rip);
            frames = frames->rbp;
        }
    }

    if (!frames->rip || !frames->rbp)
    {
        CurrentDisplay->SetPrintColor(0x2565CC);
        printf("%p", (void *)RIP);
        CurrentDisplay->SetPrintColor(0x7925CC);
        printf("-");
        CurrentDisplay->SetPrintColor(0xAA25CC);
        printf("%s", SymTbl->GetSymbolFromAddress(RIP));
        CurrentDisplay->SetPrintColor(0x7981FC);
        printf(" <- Exception");
        CurrentDisplay->SetPrintColor(0xFF0000);
        printf("\n< No stack trace available. >\n");
        CPU_HALT;
    }
    else
    {
        CurrentDisplay->SetPrintColor(0x2565CC);
        printf("%p", (void *)RIP);
        CurrentDisplay->SetPrintColor(0x7925CC);
        printf("-");
        CurrentDisplay->SetPrintColor(0xAA25CC);
        if (RIP >= 0xFFFFFFFF80000000 && RIP <= (uint64_t)&_kernel_end)
            printf("%s", SymTbl->GetSymbolFromAddress(RIP));
        else
            printf("Outside Kernel");
        CurrentDisplay->SetPrintColor(0x7981FC);
        printf(" <- Exception");
        for (uint64_t frame = 0; frame < 20; ++frame)
        {
            if (!frames->rip)
                break;
            printf("\n");
            CurrentDisplay->SetPrintColor(0x2565CC);
            printf("%p", (void *)frames->rip);
            CurrentDisplay->SetPrintColor(0x7925CC);
            printf("-");
            CurrentDisplay->SetPrintColor(0x25CCC9);
            if (frames->rip >= 0xFFFFFFFF80000000 && frames->rip <= (uint64_t)&_kernel_end)
                printf("%s", SymTbl->GetSymbolFromAddress(frames->rip));
            else
            {
                CurrentDisplay->SetPrintColor(0xFF4CA9);
                printf("Outside Kernel");
            }
            frames = frames->rbp;
        }
    }

    CPU_HALT;
}
