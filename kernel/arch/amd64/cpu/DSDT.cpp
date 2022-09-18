#if defined(__amd64__)

#include "acpi.hpp"

#include <io.h>

#include "../timer.h"
#include "apic.hpp"
#include "smp.hpp"

#include <int.h>

ACPI::DSDT *dsdt = nullptr;

#define ACPI_TIMER 0x0001
#define ACPI_BUSMASTER 0x0010
#define ACPI_GLOBAL 0x0020
#define ACPI_POWER_BUTTON 0x0100
#define ACPI_SLEEP_BUTTON 0x0200
#define ACPI_RTC_ALARM 0x0400
#define ACPI_PCIE_WAKE 0x4000
#define ACPI_WAKE 0x8000

void SCIHandler(TrapFrame *regs)
{
    debug("SCI Handle Triggered");
    uint16_t event = dsdt->GetSCIevent();
    debug("SCI Event: %#llx", event);
    if (event & ACPI_TIMER)
        event &= ~ACPI_TIMER; // remove the ACPI timer flag
    switch (event)
    {
    case ACPI_POWER_BUTTON:
        dsdt->shutdown();
        msleep(50);
        outw(0xB004, 0x2000);
        outw(0x604, 0x2000);
        outw(0x4004, 0x3400);
        CPU_STOP;
        break;
    default:
        warn("unknown event 0x%04p", event);
        return;
    }
    UNUSED(regs);
}

namespace ACPI
{
    __attribute__((always_inline)) inline bool IsCanonical(uint64_t Address)
    {
        return ((Address <= 0x00007FFFFFFFFFFF) || ((Address >= 0xFFFF800000000000) && (Address <= 0xFFFFFFFFFFFFFFFF)));
    }

#define ACPI_ENABLED 0x0001
#define ACPI_SLEEP 0x2000

#define ACPI_GAS_MMIO 0
#define ACPI_GAS_IO 1
#define ACPI_GAS_PCI 2

    void DSDT::shutdown()
    {
        trace("Shutting down...");
        if (!ACPIShutdownSupported)
        {
            outl(0xB004, 0x2000); // for qemu
            outl(0x604, 0x2000);  // if qemu not working, bochs and older versions of qemu
            outl(0x4004, 0x3400); // virtual box
        }
        else if (SCI_EN == 1)
        {
            outw(acpi->FADT->PM1aControlBlock, (inw(acpi->FADT->PM1aControlBlock) & 0xE3FF) | ((SLP_TYPa << 10) | ACPI_SLEEP));
            if (acpi->FADT->PM1bControlBlock)
                outw(acpi->FADT->PM1bControlBlock, (inw(acpi->FADT->PM1bControlBlock) & 0xE3FF) | ((SLP_TYPb << 10) | ACPI_SLEEP));
            outw(PM1a_CNT, SLP_TYPa | SLP_EN);
            if (PM1b_CNT)
                outw(PM1b_CNT, SLP_TYPb | SLP_EN);
        }
        CPU_HALT;
    }

    void DSDT::reboot()
    {
        trace("Rebooting...");
        if (!ACPIShutdownSupported)
        {
            uint8_t val = 0x02;
            while (val & 0x02)
                val = inb(0x64);
            outb(0x64, 0xFE);

            warn("Executing the second attempt to reboot...");

            // second attempt to reboot
            // https://wiki.osdev.org/Reboot
            uint8_t temp;
            asm volatile("cli");
            do
            {
                temp = inb(0x64);
                if (((temp) & (1 << (0))) != 0)
                    inb(0x60);
            } while (((temp) & (1 << (1))) != 0);
            outb(0x64, 0xFE);

            CPU_HALT;
        }
        switch (acpi->FADT->ResetReg.AddressSpace)
        {
        case ACPI_GAS_MMIO:
            *(uint8_t *)(acpi->FADT->ResetReg.Address) = acpi->FADT->ResetValue;
            break;
        case ACPI_GAS_IO:
            outb(acpi->FADT->ResetReg.Address, acpi->FADT->ResetValue);
            break;
        case ACPI_GAS_PCI:
            fixme("ACPI_GAS_PCI not handled for now");
            /*
                seg      - 0
                bus      - 0
                dev      - (FADT->ResetReg.Address >> 32) & 0xFFFF
                function - (FADT->ResetReg.Address >> 16) & 0xFFFF
                offset   - FADT->ResetReg.Address & 0xFFFF
                value    - FADT->ResetValue
            */
            break;
        }
        CPU_HALT;
    }

    uint16_t DSDT::GetSCIevent()
    {
        uint16_t a = 0, b = 0;
        if (acpi->FADT->PM1aEventBlock)
        {
            a = inw(acpi->FADT->PM1aEventBlock);
            outw(acpi->FADT->PM1aEventBlock, a);
        }
        if (acpi->FADT->PM1bEventBlock)
        {
            b = inw(acpi->FADT->PM1bEventBlock);
            outw(acpi->FADT->PM1bEventBlock, b);
        }
        return a | b;
    }

    void DSDT::SetSCIevent(uint16_t value)
    {
        uint16_t a = acpi->FADT->PM1aEventBlock + (acpi->FADT->PM1EventLength / 2);
        uint16_t b = acpi->FADT->PM1bEventBlock + (acpi->FADT->PM1EventLength / 2);
        if (acpi->FADT->PM1aEventBlock)
            outw(a, value);
        if (acpi->FADT->PM1bEventBlock)
            outw(b, value);
    }

    void DSDT::RegisterSCIEvents()
    {
        SetSCIevent(ACPI_POWER_BUTTON | ACPI_SLEEP_BUTTON | ACPI_WAKE);
        GetSCIevent();
    }

    void DSDT::InitSCI()
    {
        // this should be done for all CPUs
        if (ACPIShutdownSupported)
        {
            debug("Registering SCI Handler to vector IRQ%d", acpi->FADT->SCI_Interrupt);
            RegisterSCIEvents();
            RegisterInterrupt(SCIHandler, acpi->FADT->SCI_Interrupt + IRQ0, true, true);
        }
    }

    DSDT::DSDT()
    {
        uint64_t Address = ((IsCanonical(acpi->FADT->X_Dsdt) && acpi->XSDTSupported) ? acpi->FADT->X_Dsdt : acpi->FADT->Dsdt);
        uint8_t *S5Address = (uint8_t *)(Address) + 36;
        ACPI::ACPI::ACPIHeader *Header = (ACPI::ACPI::ACPIHeader *)Address;
        uint64_t Length = Header->Length;
        Address *= 2;
        while (Length-- > 0)
        {
            if (!memcmp(S5Address, "_S5_", 4))
                break;
            S5Address++;
        }
        if (Length <= 0)
        {
            warn("_S5 not present in ACPI");
            return;
        }
        if ((*(S5Address - 1) == 0x08 || (*(S5Address - 2) == 0x08 && *(S5Address - 1) == '\\')) && *(S5Address + 4) == 0x12)
        {
            S5Address += 5;
            S5Address += ((*S5Address & 0xC0) >> 6) + 2;
            if (*S5Address == 0x0A)
                S5Address++;
            SLP_TYPa = *(S5Address) << 10;
            S5Address++;
            if (*S5Address == 0x0A)
                S5Address++;
            SLP_TYPb = *(S5Address) << 10;
            SMI_CMD = acpi->FADT->SMI_CommandPort;
            ACPI_ENABLE = acpi->FADT->AcpiEnable;
            ACPI_DISABLE = acpi->FADT->AcpiDisable;
            PM1a_CNT = acpi->FADT->PM1aControlBlock;
            PM1b_CNT = acpi->FADT->PM1bControlBlock;
            PM1_CNT_LEN = acpi->FADT->PM1ControlLength;
            SLP_EN = 1 << 13;
            SCI_EN = 1;
            trace("ACPI Shutdown is supported");
            ACPIShutdownSupported = true;
            return;
        }
        warn("Failed to parse _S5 in ACPI");
        SCI_EN = 0;
    }

    DSDT::~DSDT()
    {
    }
}

#endif
