#include <task.h>
#include <display.h>
#include <debug.h>
#include <asm.h>
#include <cputables.h>
#include <bootscreen.h>
#include "../drivers/serial.h"

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};

EXTERNC int thread_page_fault_handler(REGISTERS *regs)
{
    uint64_t addr = 0;
    debug("Page fault triggered (%#llx)!", CS);
    addr = readcr2().raw;
    debug("Page fault at address %#llx", addr);
    if (ERROR_CODE & 0x00000008)
    {
        err("One or more page directory entries contain reserved bits which are set to 1.");
    }
    else
    {
        err(pagefault_message[ERROR_CODE & 0b111]);
    }
    PageFaultErrorCode params = {.raw = (uint32_t)ERROR_CODE};
    serial_write_text(COM1, (char *)"Page: ");
    if (params.P)
    {
        serial_write_text(COM1, (char *)"Present");
    }
    else
    {
        serial_write_text(COM1, (char *)"Not Present");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Write Operation: ");
    if (params.W)
    {
        serial_write_text(COM1, (char *)"Read-Only");
    }
    else
    {
        serial_write_text(COM1, (char *)"Read-Write");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Processor Mode: ");
    if (params.U)
    {
        serial_write_text(COM1, (char *)"User-Mode");
    }
    else
    {
        serial_write_text(COM1, (char *)"Kernel-Mode");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"CPU Reserved Bits: ");
    if (params.R)
    {
        serial_write_text(COM1, (char *)"Reserved");
    }
    else
    {
        serial_write_text(COM1, (char *)"Unreserved");
    }
    write_serial(COM1, '\n');
    serial_write_text(COM1, (char *)"Caused By An Instruction Fetch: ");
    if (params.I)
    {
        serial_write_text(COM1, (char *)"Yes");
    }
    else
    {
        serial_write_text(COM1, (char *)"No");
    }
    write_serial(COM1, '\n');

    if (CS == 0x23)
    {
        err("userspace exception");
    }
    if (CS == 0x1B)
    {
        err("Page Fault at address %#llx", addr);
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

void StartTasking(uint64_t Address, TaskingMode Mode)
{
    CLI;
    trace("Initializing Syscalls...");
    init_syscalls();
    BS->IncreaseProgres();
    trace("Starting tasking mode %d", Mode);
    switch (Mode)
    {
    case TaskingMode::Mono:
    {
        MonoTasking::SingleProcessing = new MonoTasking::MonoTasking(Address);
        BS->IncreaseProgres();
        break;
    }
    case TaskingMode::Multi:
    {
        MultiTasking::MultiProcessing = new MultiTasking::MultiTasking();
        MultiTasking::MultiProcessing->CreateThread(MultiTasking::MultiProcessing->CreateProcess(nullptr, (char *)"kernel"),
                                                    Address, 0, 0,
                                                    ControlBlockPriority::PRIORITY_REALTIME,
                                                    ControlBlockState::STATE_READY,
                                                    ControlBlockPolicy::POLICY_KERNEL);
        MultiTasking::MultiProcessing->ToggleScheduler(true);
        BS->IncreaseProgres();
        break;
    }
    default:
    {
        STI;
        throw;
        break;
    }
    }
    STI;
}
