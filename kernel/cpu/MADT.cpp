#include "acpi.hpp"
#include <heap.h>

ACPI::MADT *madt = nullptr;

namespace ACPI
{
    MADT::MADT()
    {
        LAPICAddr = (LAPIC *)(uintptr_t)acpi->MADT->LocalControllerAddress;
        for (uint8_t *ptr = (uint8_t *)(acpi->MADT->Entries);
             (uintptr_t)(ptr) < (uintptr_t)(acpi->MADT) + acpi->MADT->Header.Length;
             ptr += *(ptr + 1))
        {
            switch (*(ptr))
            {
            case 0:
            {
                if (ptr[4] & 1)
                {
                    lapic.push_back((LocalAPIC *)ptr);
                    trace("Local APIC %#llx (APIC %#llx) found.", lapic.back()->ACPIProcessorId, lapic.back()->APICId);
                    CPUCores++;
                }
                break;
            }
            case 1:
            {
                ioapic.push_back((MADTIOApic *)ptr);
                trace("I/O APIC %#llx (Address %#llx) found.", ioapic.back()->APICID, ioapic.back()->addr);
                KernelPageTableManager.MapMemory((void *)(uintptr_t)ioapic.back()->addr, (void *)(uintptr_t)ioapic.back()->addr, PTFlag::RW | PTFlag::PCD); // Make sure that the address is mapped.
                break;
            }
            case 2:
            {
                iso.push_back((MADTIso *)ptr);
                trace("ISO (IRQ:%#llx, BUS:%#llx, GSI:%#llx, %s/%s) found.",
                      iso.back()->IRQSource, iso.back()->BuSSource, iso.back()->GSI,
                      iso.back()->Flags & 0x00000004 ? "Active High" : "Active Low",
                      iso.back()->Flags & 0x00000100 ? "Edge Triggered" : "Level Triggered");
                break;
            }
            case 4:
            {
                nmi.push_back((MADTNmi *)ptr);
                trace("NMI %#llx (lint:%#llx) found.", nmi.back()->processor, nmi.back()->lint);
                break;
            }
            case 5:
            {
                LAPICAddr = (LAPIC *)ptr;
                trace("APIC found at %#llx", LAPICAddr);
                break;
            }
            }
            KernelPageTableManager.MapMemory((void *)LAPICAddr, (void *)LAPICAddr, PTFlag::RW | PTFlag::PCD); // I should map more than one page?
        }
        trace("Total CPU cores: %d", CPUCores);
    }

    MADT::~MADT()
    {
    }
}
