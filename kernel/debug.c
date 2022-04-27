#include <debug.h>
#include <stdarg.h>
#include <interrupts.h>
#include <cputables.h>
#include <asm.h>
#include <printf.h>
#include <lock.h>
#include "drivers/serial.h"

void dbg_showregs(void *r, const char *file, int line, const char *function)
{
    REGISTERS *regs = r;
    CR0 cr0 = readcr0();
    CR2 cr2 = readcr2();
    CR3 cr3 = readcr3();
    CR4 cr4 = readcr4();
    CR8 cr8 = readcr8();
    RFLAGS rflags = FLAGS;
    err("\tFS =%#lx  GS =%#lx  SS =%#lx  CS =%#lx", rdmsr(MSR_FS_BASE), rdmsr(MSR_GS_BASE), _SS, CS);
    err("\tR8 =%#lx  R9 =%#lx  R10=%#lx  R11=%#lx", R8, R9, R10, R11);
    err("\tR12=%#lx  R13=%#lx  R14=%#lx  R15=%#lx", R12, R13, R14, R15);
    err("\tRAX=%#lx  RBX=%#lx  RCX=%#lx  RDX=%#lx", RAX, RBX, RCX, RDX);
    err("\tRSI=%#lx  RDI=%#lx  RBP=%#lx  RSP=%#lx", RSI, RDI, RBP, RSP);
    err("\tRIP=%#lx  RFL=%#lx  DS =%#lx  INT=%#lx  ERR=%#lx", RIP, FLAGS.raw, DS, INT_NUM, ERROR_CODE);
    err("\tCR0=%#lx  CR2=%#lx  CR3=%#lx  CR4=%#lx  CR8=%#lx", cr0.raw, cr2.raw, cr3.raw, cr4.raw, cr8.raw);

    err("\nCR0: PE:%s     MP:%s     EM:%s     TS:%s\n     ET:%s     NE:%s     WP:%s     AM:%s\n     NW:%s     CD:%s     PG:%s\n     R0:%#x R1:%#x R2:%#x",
        cr0.PE ? "True " : "False", cr0.MP ? "True " : "False", cr0.EM ? "True " : "False", cr0.TS ? "True " : "False",
        cr0.ET ? "True " : "False", cr0.NE ? "True " : "False", cr0.WP ? "True " : "False", cr0.AM ? "True " : "False",
        cr0.NW ? "True " : "False", cr0.CD ? "True " : "False", cr0.PG ? "True " : "False",
        cr0._reserved0, cr0._reserved1, cr0._reserved2);

    err("\nCR2: PFLA: %#lx", cr2.PFLA);

    err("\nCR3: PWT:%s     PCD:%s    PDBR:%#lx", cr3.PWT ? "True " : "False", cr3.PCD ? "True " : "False", cr3.PDBR);

    err("\nCR4: VME:%s     PVI:%s     TSD:%s      DE:%s\n     PSE:%s     PAE:%s     MCE:%s     PGE:%s\n     PCE:%s    UMIP:%s  OSFXSR:%s OSXMMEXCPT:%s\n    LA57:%s    VMXE:%s    SMXE:%s   PCIDE:%s\n OSXSAVE:%s    SMEP:%s    SMAP:%s     PKE:%s\n     R0:%d R1:%d R2:%d",
        cr4.VME ? "True " : "False", cr4.PVI ? "True " : "False", cr4.TSD ? "True " : "False", cr4.DE ? "True " : "False",
        cr4.PSE ? "True " : "False", cr4.PAE ? "True " : "False", cr4.MCE ? "True " : "False", cr4.PGE ? "True " : "False",
        cr4.PCE ? "True " : "False", cr4.UMIP ? "True " : "False", cr4.OSFXSR ? "True " : "False", cr4.OSXMMEXCPT ? "True " : "False",
        cr4.LA57 ? "True " : "False", cr4.VMXE ? "True " : "False", cr4.SMXE ? "True " : "False", cr4.PCIDE ? "True " : "False",
        cr4.OSXSAVE ? "True " : "False", cr4.SMEP ? "True " : "False", cr4.SMAP ? "True " : "False", cr4.PKE ? "True " : "False",
        cr4._reserved0, cr4._reserved1, cr4._reserved2);

    err("\nCR8: TPL:%d", cr8.TPL);

    err("\nRFL: CF:%s     PF:%s     AF:%s     ZF:%s\n     SF:%s     TF:%s     IF:%s     DF:%s\n     OF:%s   IOPL:%s     NT:%s     RF:%s\n     VM:%s     AC:%s    VIF:%s    VIP:%s\n     ID:%s     AlwaysOne:%d\n     R0:%#x R1:%#x R2:%#x R3:%#x",
        FLAGS.CF ? "True " : "False", FLAGS.PF ? "True " : "False", FLAGS.AF ? "True " : "False", FLAGS.ZF ? "True " : "False",
        FLAGS.SF ? "True " : "False", FLAGS.TF ? "True " : "False", FLAGS.IF ? "True " : "False", FLAGS.DF ? "True " : "False",
        FLAGS.OF ? "True " : "False", FLAGS.IOPL ? "True " : "False", FLAGS.NT ? "True " : "False", FLAGS.RF ? "True " : "False",
        FLAGS.VM ? "True " : "False", FLAGS.AC ? "True " : "False", FLAGS.VIF ? "True " : "False", FLAGS.VIP ? "True " : "False",
        FLAGS.ID ? "True " : "False", FLAGS.always_one,
        FLAGS._reserved0, FLAGS._reserved1, FLAGS._reserved2, FLAGS._reserved3);
}

static inline void serialwrite_wrapper(char c, void *unused)
{
    (void)unused;
    write_serial(COM1, c);
}

int dbg_vprintf(const char *format, va_list list)
{
    return vfctprintf(serialwrite_wrapper, NULL, format, list);
}

void shortdbgprint(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    dbg_vprintf(format, args);
    va_end(args);
}

int debug_printf(enum debug_type type, const char *message, const char *file, int line, const char *function, ...)
{
    switch (type)
    {
    case _fixme:
        serial_write_text(COM1, "FIXME");
        break;
    case _stub:
        serial_write_text(COM1, "STUB");
        break;
    case _trace:
        serial_write_text(COM1, "TRACE");
        break;
    case _warn:
        serial_write_text(COM1, "WARN");
        break;
    case _err:
        serial_write_text(COM1, "ERROR");
        break;
    case _debug:
        serial_write_text(COM1, "DEBUG");
        break;
    case _ubsan:
        serial_write_text(COM1, "UBSAN");
        break;
    case _checkpoint:
        serial_write_text(COM1, "Checkpoint hit in ");
        serial_write_text(COM1, (char *)file);
        serial_write_text(COM1, " at ");
        shortdbgprint("%d", line);
        serial_write_text(COM1, " in function \"");
        serial_write_text(COM1, (char *)function);
        serial_write_text(COM1, "\"\n");
        goto cleanup;
    default:
        serial_write_text(COM1, "MESSAGE");
        break;
    }
    serial_write_text(COM1, ":");
    serial_write_text(COM1, (char *)file);
    serial_write_text(COM1, ":");
    shortdbgprint("%d", line);
    serial_write_text(COM1, ":");
    serial_write_text(COM1, (char *)function);
    if (type != _stub)
    {
        serial_write_text(COM1, ": ");
        va_list args;
        va_start(args, message);
        dbg_vprintf(message, args);
        serial_write_text(COM1, "\n");
        va_end(args);
    }
    else
    {
        serial_write_text(COM1, ": stub!\n");
    }
cleanup:
    if (type == _stub)
    {
        return -1;
    }
    return 0;
}
