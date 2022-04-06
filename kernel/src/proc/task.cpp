#include <task.h>
#include <display.h>
#include <debug.h>
#include <asm.h>
#include "../drivers/serial.h"

typedef union _PAGE_FAULT_PARAMS_T
{
    struct
    {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user : 1;
        uint32_t reserved_write : 1;
        uint32_t instruction_fetch : 1;
    };
    uint32_t raw;
} PAGE_FAULT_PARAMS_T;

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};
#define VFS_MOUNTROOTNAME "[ROOT]"

EXTERNC int thread_page_fault_handler(REGISTERS *regs)
{
    uint64_t addr = 0;
    debug("page fault triggered (%#x)!!!!\nTo get the address of the fault type \"objdump -x kernel.fsys\" where is the file", CS);
    addr = readcr2();
    debug("             make search WHAT=%x\n\t\tFaulty address (if is not in kernel): %016p", addr, addr);
    if (ERROR_CODE & 0x00000008)
    {
        err("One or more page directory entries contain reserved bits which are set to 1.");
    }
    else
    {
        err(pagefault_message[ERROR_CODE & 0b111]);
    }
    PAGE_FAULT_PARAMS_T params = {.raw = (uint32_t)ERROR_CODE};
    // it can be obtained from this but the union typedef will do the same
    // int present = !(ERROR_CODE & BIT0);
    // int rw = ERROR_CODE & BIT1;
    // int us = ERROR_CODE & BIT2;
    // int reserved = ERROR_CODE & BIT3;
    // int id = ERROR_CODE & BIT4;
    serial_write_text(COM1, (char *)"Page: "); // if the page is present or not
    if (params.present)
    {
        serial_write_text(COM1, (char *)"P");
    }
    else
    {
        serial_write_text(COM1, (char *)"Not P");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Write Operation: "); // it was read only or write
    if (params.write)
    {
        serial_write_text(COM1, (char *)"Read-Only");
    }
    else
    {
        serial_write_text(COM1, (char *)"Read-Write");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Processor Mode: "); // user mode or kernel mode
    if (params.user)
    {
        serial_write_text(COM1, (char *)"User-Mode");
    }
    else
    {
        serial_write_text(COM1, (char *)"Kernel-Mode");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"CPU Reserved Bits: "); // overwrite cpu reserved pages of page entry?
    if (params.reserved_write)
    {
        serial_write_text(COM1, (char *)"Reserved");
    }
    else
    {
        serial_write_text(COM1, (char *)"Unreserved");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Caused By An Instruction Fetch: "); // does instruction fetch was the problem?
    if (params.instruction_fetch)
    {
        serial_write_text(COM1, (char *)"Yes");
    }
    else
    {
        serial_write_text(COM1, (char *)"No");
    }
    write_serial(COM1, '\n');

    // debug("CR2:%#x-V:%#x-P:%#x", addr, (addr + KERNEL_VMA), (addr - KERNEL_VMA)); // TODO FIX
    if (CS == 0x23)
    {
        err("userspace exception");
    }
    if (CS == 0x1B)
    {
        err("Page Fault at address %#x", addr);
        // if (addr == 0x00000000FFFFFFFF)
        //     do_exit(RAX);
        // else if (addr == (uint64_t)do_exit) // sigreturn
        //     do_exit(RAX);                   // sigreturn(r);
        // else
        //     goto return_err;
        // return 0;
    }
return_err:
    // if (panic_success != 0)
    //     if (panic_success != 2)
    //     {
    //         err("Very bad page fault occurred!");
    //         CPU_STOP;
    //     }
    return -1; // not good
}
