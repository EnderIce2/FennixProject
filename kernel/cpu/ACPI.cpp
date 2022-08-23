#include "acpi.hpp"
#include "../kernel.h"

#include <io.h>

#include "../pci.h"

ACPI::ACPI *acpi = nullptr;

namespace ACPI
{
    void *ACPI::FindTable(ACPI::ACPIHeader *ACPIHeader, char *Signature)
    {
        for (uint64_t t = 0; t < ((ACPIHeader->Length - sizeof(ACPI::ACPIHeader)) / (XSDTSupported ? 8 : 4)); t++)
        {
            // Should I be concerned about unaligned memory access?
            ACPI::ACPIHeader *SDTHdr = nullptr;
            if (XSDTSupported)
                SDTHdr = (ACPI::ACPIHeader *)(*(uint64_t *)((uint64_t)ACPIHeader + sizeof(ACPI::ACPIHeader) + (t * 8)));
            else
                SDTHdr = (ACPI::ACPIHeader *)(*(uint32_t *)((uint64_t)ACPIHeader + sizeof(ACPI::ACPIHeader) + (t * 4)));

            for (uint64_t i = 0; i < 4; i++)
            {
                if (SDTHdr->Signature[i] != Signature[i])
                    break;
                if (i == 3)
                {
                    trace("%s found!", Signature);
                    return SDTHdr;
                }
            }
        }
        warn("%s not found!", Signature);
        return 0;
    }

    void ACPI::SearchTables(ACPIHeader *Header)
    {
        if (!Header)
            return;

        HPET = (HPETHeader *)FindTable(XSDT, (char *)"HPET");
        FADT = (FADTHeader *)FindTable(XSDT, (char *)"FACP");
        MCFG = (MCFGHeader *)FindTable(XSDT, (char *)"MCFG");
        BGRT = (BGRTHeader *)FindTable(XSDT, (char *)"BGRT");
        SRAT = (SRATHeader *)FindTable(XSDT, (char *)"SRAT");
        TPM2 = (TPM2Header *)FindTable(XSDT, (char *)"TPM2");
        TCPA = (TCPAHeader *)FindTable(XSDT, (char *)"TCPA");
        WAET = (WAETHeader *)FindTable(XSDT, (char *)"WAET");
        MADT = (MADTHeader *)FindTable(XSDT, (char *)"APIC");
        HEST = (HESTHeader *)FindTable(XSDT, (char *)"HEST");
    }

    ACPI::ACPI()
    {
        trace("Initializing ACPI");
        if (bootparams->rsdp->Revision >= 2 && bootparams->rsdp->XSDTAddress)
        {
            debug("XSDT supported");
            XSDTSupported = true;
            XSDT = (ACPIHeader *)(bootparams->rsdp->XSDTAddress);
        }
        else
        {
            debug("RSDT supported");
            XSDT = (ACPIHeader *)(uintptr_t)bootparams->rsdp->RSDTAddress;
        }

        this->SearchTables(XSDT);

        outb(FADT->SMI_CommandPort, FADT->AcpiEnable);
        while (!(inw(FADT->PM1aControlBlock) & 1))
            ;
    }

    ACPI::~ACPI()
    {
    }
}
