#if defined(__amd64__)

#include "acpi.hpp"
#include "../kernel.h"

#include <io.h>
#include <bootscreen.h>

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
        // warn("%s not found!", Signature);
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
        FindTable(XSDT, (char *)"BERT");
        FindTable(XSDT, (char *)"CPEP");
        FindTable(XSDT, (char *)"DSDT");
        FindTable(XSDT, (char *)"ECDT");
        FindTable(XSDT, (char *)"EINJ");
        FindTable(XSDT, (char *)"ERST");
        FindTable(XSDT, (char *)"FACS");
        FindTable(XSDT, (char *)"MSCT");
        FindTable(XSDT, (char *)"MPST");
        FindTable(XSDT, (char *)"OEMx");
        FindTable(XSDT, (char *)"PMTT");
        FindTable(XSDT, (char *)"PSDT");
        FindTable(XSDT, (char *)"RASF");
        FindTable(XSDT, (char *)"RSDT");
        FindTable(XSDT, (char *)"SBST");
        FindTable(XSDT, (char *)"SLIT");
        FindTable(XSDT, (char *)"SSDT");
        FindTable(XSDT, (char *)"XSDT");
        FindTable(XSDT, (char *)"DRTM");
        FindTable(XSDT, (char *)"FPDT");
        FindTable(XSDT, (char *)"GTDT");
        FindTable(XSDT, (char *)"PCCT");
        FindTable(XSDT, (char *)"S3PT");
        FindTable(XSDT, (char *)"MATR");
        FindTable(XSDT, (char *)"MSDM");
        FindTable(XSDT, (char *)"WPBT");
        FindTable(XSDT, (char *)"OSDT");
        FindTable(XSDT, (char *)"RSDP");
        FindTable(XSDT, (char *)"NFIT");
        FindTable(XSDT, (char *)"ASF!");
        FindTable(XSDT, (char *)"BOOT");
        FindTable(XSDT, (char *)"CSRT");
        FindTable(XSDT, (char *)"DBG2");
        FindTable(XSDT, (char *)"DBGP");
        FindTable(XSDT, (char *)"DMAR");
        FindTable(XSDT, (char *)"IBFT");
        FindTable(XSDT, (char *)"IORT");
        FindTable(XSDT, (char *)"IVRS");
        FindTable(XSDT, (char *)"LPIT");
        FindTable(XSDT, (char *)"MCHI");
        FindTable(XSDT, (char *)"MTMR");
        FindTable(XSDT, (char *)"SLIC");
        FindTable(XSDT, (char *)"SPCR");
        FindTable(XSDT, (char *)"SPMI");
        FindTable(XSDT, (char *)"UEFI");
        FindTable(XSDT, (char *)"VRTC");
        FindTable(XSDT, (char *)"WDAT");
        FindTable(XSDT, (char *)"WDDT");
        FindTable(XSDT, (char *)"WDRT");
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
        BS->DrawVendorLogo(BGRT);

        outb(FADT->SMI_CommandPort, FADT->AcpiEnable);
        while (!(inw(FADT->PM1aControlBlock) & 1))
            ;
    }

    ACPI::~ACPI()
    {
    }
}

#endif
