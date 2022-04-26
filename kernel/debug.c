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
    // CR2 cr2 = readcr2();
    // CR3 cr3 = readcr3();
    CR4 cr4 = readcr4();
    // CR4 cr8 = readcr8();
    RFLAGS rflags = FLAGS;
    debug("\n============================= REGS =============================\nBASIC REGISTERS:\n  FS =%#llx  GS =%#llx  SS =%#llx  CS =%#llx\n  CR0=%#llx  CR2=%#llx  CR3=%#llx  CR4=%#llx\n  R8 =%#llx  R9 =%#llx  R10=%#llx  R11=%#llx\n  R12=%#llx  R13=%#llx  R14=%#llx  R15=%#llx\n  RAX=%#llx  RBX=%#llx  RCX=%#llx  RDX=%#llx\n  RSI=%#llx  RDI=%#llx  RBP=%#llx  RSP=%#llx\n  RIP=%#llx  RFL=%#llx\n  DS =%#llx  INT=%#llx  ERR=%#llx\nRFLAGS:\n  CF =%01p  PF =%01p  AF =%01p  ZF =%01p\n  SF =%01p  TF =%01p  IF =%01p  DF =%01p\n  OF =%01p  NT =%01p  RF =%01p  VM =%01p\n  AC =%01p  ID =%01p  VIF=%01p  VIP=%01p\n  R1 =%01p  R2 =%01p  R3 =%01p  R4 =%010p\n  ALWAYS_ONE=%01p  IOPL=%02p\nCR0 REGISTERS:\n  PE=%01p   MP=%01p   EM=%01p   TS=%01p\n  ET=%01p   NE=%01p   R0=%010p   WP=%01p\n  R1=%01p   AM=%01p   R2=%010p   NW=%01p\n  CD=%01p   PG=%01p\nCR4 REGISTERS:\n  VME=%01p  PVI=%01p  TSD=%01p  DE =%01p\n  PSE=%01p  PAE=%01p  MCE=%01p  PGE=%01p\n  PCE=%01p  PKE=%01p  R3 =%01p  R4 =%01p  R5 =%09p\n  SMEP=%01p  SMAP=%01p  UMIP=%01p  LA57=%01p  \n  OSXSAVE=%01p  OSFXSR=%01p  OSXMMEXCPT=%01p  FSGSBASE=%01p\n  VMXE=%01p  SMXE=%01p  PCIDE=%01p\n================================================================",
          rdmsr(MSR_FS_BASE), rdmsr(MSR_GS_BASE), _SS, CS, cr0.raw, readcr2(), readcr3(), cr4.raw, R8, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, FLAGS.raw, DS, INT_NUM, ERROR_CODE,
          rflags.CF, rflags.PF, rflags.AF, rflags.ZF, rflags.SF, rflags.TF, rflags.IF, rflags.DF, rflags.OF, rflags.NT, rflags.RF, rflags.VM, rflags.AC, rflags.ID, rflags.VIF, rflags.VIP, rflags._reserved0, rflags._reserved1, rflags._reserved2, rflags._reserved3, rflags.always_one, rflags.IOPL,
          cr0.PE, cr0.MP, cr0.EM, cr0.TS, cr0.ET, cr0.NE, cr0._reserved0, cr0.WP, cr0._reserved1, cr0.AM, cr0._reserved2, cr0.NW, cr0.CD, cr0.PG,
          cr4.VME, cr4.PVI, cr4.TSD, cr4.DE, cr4.PSE, cr4.PAE, cr4.MCE, cr4.PGE, cr4.PCE, cr4.PKE, cr4._reserved0, cr4._reserved1, cr4._reserved2, cr4.SMEP, cr4.SMAP, cr4.UMIP, cr4.LA57, cr4.OSXSAVE, cr4.OSFXSR, cr4.OSXMMEXCPT, cr4.FSGSBASE, cr4.VMXE, cr4.SMXE, cr4.PCIDE);
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
