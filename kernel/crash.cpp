#include <sys.h>
#include <types.h>
#include <display.h>
#include <string.h>
#include <asm.h>
#include <heap.h>
#include <task.h>

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

EXTERNC void crash(string message)
{
    // TODO: Add more useful information.
    CLI;
    debug("System crashed with message: %s", message);
    CurrentDisplay->Clear(0xFF221160);

    CurrentDisplay->SetPrintColor(0xFFdd2920);
    SET_PRINT_MID((char *)"System crashed!", FHeight(1));
    CurrentDisplay->ResetPrintColor();
    SET_PRINT_MID((char *)message, (CurrentDisplay->GetFramebuffer()->Height / 2));
    CPU_STOP;
}

EXTERNC void isrcrash(REGISTERS *regs)
{
    CLI;
    bool isrcrashcritical = true;
    CR0 cr0 = readcr0();
    CR2 cr2 = readcr2();
    CR3 cr3 = readcr3();
    CR4 cr4 = readcr4();
    CR8 cr8 = readcr8();
    if (CS != 0x23)
        CurrentDisplay->Clear(0xFF000000);
    switch (INT_NUM)
    {
    case ISR_DivideByZero:
        if (CS == 0x23)
        {
        }
        else
        {
        }
        break;
    case ISR_Debug:
        SET_PRINT_MID((char *)"Manual Triggered Crash (Debug)", FHeight(2));
        if (CS == 0x23)
            return;
        break;
    case ISR_NonMaskableInterrupt:
        break;
    case ISR_Breakpoint:
        break;
    case ISR_Overflow:
        break;
    case ISR_BoundRange:
        break;
    case ISR_InvalidOpcode:
        break;
    case ISR_DeviceNotAvailable:
        break;
    case ISR_DoubleFault:
        break;
    case ISR_CoprocessorSegmentOverrun:
        break;
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
        SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        break;
    }
    case ISR_GeneralProtectionFault:
    {
        if (CS == 0x23)
        {
            err("General Protection Fault caused by an user-mode process.");
            // TODO: signal the application to stop.
            SysGetCurrentThread()->State = STATE_TERMINATED;
            return;
        }
        else
        {
            SelectorErrorCode SelCode = {.raw = ERROR_CODE};
        }
        break;
    }
    case ISR_PageFault:
    {
        if (CS == 0x23)
        {
            err("Page fault caused by an user-mode process.");
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
            SysGetCurrentThread()->State = STATE_TERMINATED;
            return;
        }
        else
        {
            err("Kernel Exception");
            CurrentDisplay->Clear(0xFF021c07);
            PageFaultErrorCode params = {.raw = (uint32_t)ERROR_CODE};

            // We can't use an allocator in exceptions (because that can cause another exception!) so, we'll just use a static buffer.
            char ret_err[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_present[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_write[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_user[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_reserved[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_fetch[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_protection[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_shadow[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
            char page_sgx[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

            CurrentDisplay->SetPrintColor(0xFFdd2920);
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
        }
        // TODO: check if the exception is critical and if so, halt the system. Otherwise, continue by setting isrcrashcritical to false.
        break;
    }
    case ISR_x87FloatingPoint:
        break;
    case ISR_AlignmentCheck:
        break;
    case ISR_MachineCheck:
        break;
    case ISR_SIMDFloatingPoint:
        break;
    case ISR_Virtualization:
        break;
    case ISR_Security:
        break;
    default:
        CurrentDisplay->Clear(0xFF221160);
        break;
    }

    CurrentDisplay->ResetPrintPosition();
    CurrentDisplay->SetPrintColor(0xFF7981fc);
    printf("Technical Informations:\n");
    printf("FS =%#lx  GS =%#lx  SS =%#lx  CS =%#lx\n", rdmsr(MSR_FS_BASE), rdmsr(MSR_GS_BASE), _SS, CS);
    printf("R8 =%#lx  R9 =%#lx  R10=%#lx  R11=%#lx\n", R8, R9, R10, R11);
    printf("R12=%#lx  R13=%#lx  R14=%#lx  R15=%#lx\n", R12, R13, R14, R15);
    printf("RAX=%#lx  RBX=%#lx  RCX=%#lx  RDX=%#lx\n", RAX, RBX, RCX, RDX);
    printf("RSI=%#lx  RDI=%#lx  RBP=%#lx  RSP=%#lx\n", RSI, RDI, RBP, RSP);
    printf("RIP=%#lx  RFL=%#lx  DS =%#lx  INT=%#lx  ERR=%#lx\n", RIP, FLAGS.raw, DS, INT_NUM, ERROR_CODE);
    printf("CR0=%#lx  CR2=%#lx  CR3=%#lx  CR4=%#lx  CR8=%#lx\n", cr0.raw, cr2.raw, cr3.raw, cr4.raw, cr8.raw);

    CurrentDisplay->SetPrintColor(0xFFfc797b);
    printf("CR0: PE:%s     MP:%s     EM:%s     TS:%s\n     ET:%s     NE:%s     WP:%s     AM:%s\n     NW:%s     CD:%s     PG:%s\n     R0:%#x R1:%#x R2:%#x\n",
           cr0.PE ? "True " : "False", cr0.MP ? "True " : "False", cr0.EM ? "True " : "False", cr0.TS ? "True " : "False",
           cr0.ET ? "True " : "False", cr0.NE ? "True " : "False", cr0.WP ? "True " : "False", cr0.AM ? "True " : "False",
           cr0.NW ? "True " : "False", cr0.CD ? "True " : "False", cr0.PG ? "True " : "False",
           cr0._reserved0, cr0._reserved1, cr0._reserved2);
    CurrentDisplay->SetPrintColor(0xFFfcdb79);
    printf("CR2: PFLA: %#lx\n",
           cr2.PFLA);
    CurrentDisplay->SetPrintColor(0xFF79fc84);
    printf("CR3: PWT:%s     PCD:%s    PDBR:%#lx\n",
           cr3.PWT ? "True " : "False", cr3.PCD ? "True " : "False", cr3.PDBR);
    CurrentDisplay->SetPrintColor(0xFFbd79fc);
    printf("CR4: VME:%s     PVI:%s     TSD:%s      DE:%s\n     PSE:%s     PAE:%s     MCE:%s     PGE:%s\n     PCE:%s    UMIP:%s  OSFXSR:%s OSXMMEXCPT:%s\n    LA57:%s    VMXE:%s    SMXE:%s   PCIDE:%s\n OSXSAVE:%s    SMEP:%s    SMAP:%s     PKE:%s\n     R0:%d R1:%d R2:%d\n",
           cr4.VME ? "True " : "False", cr4.PVI ? "True " : "False", cr4.TSD ? "True " : "False", cr4.DE ? "True " : "False",
           cr4.PSE ? "True " : "False", cr4.PAE ? "True " : "False", cr4.MCE ? "True " : "False", cr4.PGE ? "True " : "False",
           cr4.PCE ? "True " : "False", cr4.UMIP ? "True " : "False", cr4.OSFXSR ? "True " : "False", cr4.OSXMMEXCPT ? "True " : "False",
           cr4.LA57 ? "True " : "False", cr4.VMXE ? "True " : "False", cr4.SMXE ? "True " : "False", cr4.PCIDE ? "True " : "False",
           cr4.OSXSAVE ? "True " : "False", cr4.SMEP ? "True " : "False", cr4.SMAP ? "True " : "False", cr4.PKE ? "True " : "False",
           cr4._reserved0, cr4._reserved1, cr4._reserved2);
    CurrentDisplay->SetPrintColor(0xFF79fcf5);
    printf("CR8: TPL:%d\n", cr8.TPL);
    CurrentDisplay->SetPrintColor(0xFFfcfc02);
    printf("RFL: CF:%s     PF:%s     AF:%s     ZF:%s\n     SF:%s     TF:%s     IF:%s     DF:%s\n     OF:%s   IOPL:%s     NT:%s     RF:%s\n     VM:%s     AC:%s    VIF:%s    VIP:%s\n     ID:%s     AlwaysOne:%d\n     R0:%#x R1:%#x R2:%#x R3:%#x",
           FLAGS.CF ? "True " : "False", FLAGS.PF ? "True " : "False", FLAGS.AF ? "True " : "False", FLAGS.ZF ? "True " : "False",
           FLAGS.SF ? "True " : "False", FLAGS.TF ? "True " : "False", FLAGS.IF ? "True " : "False", FLAGS.DF ? "True " : "False",
           FLAGS.OF ? "True " : "False", FLAGS.IOPL ? "True " : "False", FLAGS.NT ? "True " : "False", FLAGS.RF ? "True " : "False",
           FLAGS.VM ? "True " : "False", FLAGS.AC ? "True " : "False", FLAGS.VIF ? "True " : "False", FLAGS.VIP ? "True " : "False",
           FLAGS.ID ? "True " : "False", FLAGS.always_one,
           FLAGS._reserved0, FLAGS._reserved1, FLAGS._reserved2, FLAGS._reserved3);

    CPU_STOP;
}
