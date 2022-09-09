#include "hpet.h"
#include <io.h>
#include "../cpu/acpi.hpp"
#include <heap.h>

bool HPET_initialized = false;
struct HPET *hpet;
static uint32_t clk = 0;

uint32_t get_clk()
{
    return clk;
}

uint64_t hpet_read_counter()
{
    return mminq(&hpet->main_counter_value);
}

void hpet_uwait(uint64_t Microseconds)
{
    uint64_t target = hpet_read_counter() + (Microseconds * 1000000000) / clk;
    while (hpet_read_counter() < target)
        ;
}

void hpet_mwait(uint64_t Miliseconds)
{
    hpet_uwait(Miliseconds * 10000);
}

void hpet_wait(uint64_t Seconds)
{
    hpet_uwait(Seconds * 1000000);
}

void init_HPET()
{
    trace("Initializing HPET timer");
    if (!acpi->HPET)
    {
        warn("HPET timer is not supported");
        return;
    }
    MapMemory(NULL, (void *)acpi->HPET->address.Address, (void *)acpi->HPET->address.Address, RW | PCD);
    hpet = (HPET *)(acpi->HPET->address.Address);
    debug("%s timer is at address %016p", acpi->HPET->header.OEMID, (void *)acpi->HPET->address.Address);
    clk = hpet->general_capabilities >> 32;
    mmoutq(&hpet->general_configuration, 0);
    mmoutq(&hpet->main_counter_value, 0);
    mmoutq(&hpet->general_configuration, 1);
    HPET_initialized = true;
}

void HPET_oneshot(uint32_t Vector, uint64_t Miliseconds)
{
    err("HPET One-Shot is not implemented!");
}

void disable_HPET()
{
    HPET_initialized = false;
}