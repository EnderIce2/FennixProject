#include "acpi.h"
#include "kernel.h"

#include <io.h>
#include <asm.h>
#include <string.h>
#include <heap.h>

#include "timer.h"
#include "pci.h"

__attribute__((always_inline)) inline bool IsCanonical(uint64_t Address)
{
    return ((Address <= 0x00007FFFFFFFFFFF) || ((Address >= 0xFFFF800000000000) && (Address <= 0xFFFFFFFFFFFFFFFF)));
}

uint32_t SMI_CMD;
uint8_t ACPI_ENABLE;
uint8_t ACPI_DISABLE;
uint32_t PM1a_CNT;
uint32_t PM1b_CNT;
uint16_t SLP_TYPa;
uint16_t SLP_TYPb;
uint16_t SLP_EN;
uint16_t SCI_EN;
uint8_t PM1_CNT_LEN;
bool XSDT_supported = false;

struct ACPIHeader *XSDT = NULL;
struct MCFGHeader *MCFG = NULL;
struct HPETHeader *HPET = NULL;
struct FADTHeader *FADT = NULL;
struct BGRTHeader *BGRT = NULL;
struct SRATHeader *SRAT = NULL;
struct TPM2Header *TPM2 = NULL;
struct TCPAHeader *TCPA = NULL;
struct WAETHeader *WAET = NULL;
struct MADTHeader *MADT = NULL;

struct LocalAPIC *LAPIC[MADT_MAX_SIZE];
struct MADTIOApic *IOAPIC[MADT_MAX_SIZE];
struct MADTIso *ISO[MADT_MAX_SIZE];
struct MADTNmi *NMI[MADT_MAX_SIZE];
struct LAPIC *LAPICAddr;

apics apicsnum = {0, 0, 0, 0};
uint8_t CPUCores;

#define ACPI_ENABLED 0x0001
#define ACPI_SLEEP 0x2000

void shutdown()
{
    if (SCI_EN == 1)
    {
        outw(FADT->PM1aControlBlock, (inw(FADT->PM1aControlBlock) & 0xE3FF) | ((SLP_TYPa << 10) | ACPI_SLEEP));
        if (FADT->PM1bControlBlock)
            outw(FADT->PM1bControlBlock, (inw(FADT->PM1bControlBlock) & 0xE3FF) | ((SLP_TYPb << 10) | ACPI_SLEEP));
        outw(PM1a_CNT, SLP_TYPa | SLP_EN);
        if (PM1b_CNT)
            outw(PM1b_CNT, SLP_TYPb | SLP_EN);
        HLT;
    }
}

#define ACPI_GAS_MMIO 0
#define ACPI_GAS_IO 1
#define ACPI_GAS_PCI 2

void reboot()
{
    switch (FADT->ResetReg.AddressSpace)
    {
    case ACPI_GAS_MMIO:
        *(uint8_t *)(FADT->ResetReg.Address) = FADT->ResetValue;
        break;
    case ACPI_GAS_IO:
        outb(FADT->ResetReg.Address, FADT->ResetValue);
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
}

uint16_t GetSCIevent()
{
    uint16_t a = 0, b = 0;
    if (FADT->PM1aEventBlock)
    {
        a = inw(FADT->PM1aEventBlock);
        outw(FADT->PM1aEventBlock, a);
    }
    if (FADT->PM1bEventBlock)
    {
        b = inw(FADT->PM1bEventBlock);
        outw(FADT->PM1bEventBlock, b);
    }
    return a | b;
}

void SetSCIevent(uint16_t value)
{
    uint16_t a = FADT->PM1aEventBlock + (FADT->PM1EventLength / 2);
    uint16_t b = FADT->PM1bEventBlock + (FADT->PM1EventLength / 2);
    if (FADT->PM1aEventBlock)
        outw(a, value);
    if (FADT->PM1bEventBlock)
        outw(b, value);
}

void SCIHandler(REGISTERS *regs)
{
    debug("SCI Handle Triggered");
    uint16_t event = GetSCIevent();
    debug("SCI Event: 0x%04p", event);
    if (event & ACPI_TIMER)
        event &= ~ACPI_TIMER; // remove the ACPI timer flag
    switch (event)
    {
    case ACPI_POWER_BUTTON:
        shutdown();
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

void RegisterSCIEvents()
{
    SetSCIevent(ACPI_POWER_BUTTON | ACPI_SLEEP_BUTTON | ACPI_WAKE);
    GetSCIevent();
}

void dsdt_init()
{
    uint64_t dsdtaddr = ((IsCanonical(FADT->X_Dsdt) && XSDT_supported) ? FADT->X_Dsdt : FADT->Dsdt);
    uint8_t *S5Addr = (uint8_t *)(dsdtaddr) + 36;
    struct ACPIHeader *dasdtstruct = (struct ACPIHeader *)dsdtaddr;
    uint64_t dsdtlength = dasdtstruct->Length;
    dsdtaddr *= 2;
    while (dsdtlength-- > 0)
    {
        if (!memcmp(S5Addr, "_S5_", 4))
            break;
        S5Addr++;
    }
    if (dsdtlength <= 0)
    {
        warn("_S5 not present in ACPI");
        return;
    }
    if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12)
    {
        S5Addr += 5;
        S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;
        if (*S5Addr == 0x0A)
            S5Addr++;
        SLP_TYPa = *(S5Addr) << 10;
        S5Addr++;
        if (*S5Addr == 0x0A)
            S5Addr++;
        SLP_TYPb = *(S5Addr) << 10;
        SMI_CMD = FADT->SMI_CommandPort;
        ACPI_ENABLE = FADT->AcpiEnable;
        ACPI_DISABLE = FADT->AcpiDisable;
        PM1a_CNT = FADT->PM1aControlBlock;
        PM1b_CNT = FADT->PM1bControlBlock;
        PM1_CNT_LEN = FADT->PM1ControlLength;
        SLP_EN = 1 << 13;
        SCI_EN = 1;
        trace("ACPI Shutdown is supported");
        return;
    }
    warn("Failed to parse _S5 in ACPI");
    SCI_EN = 0;
}

void madt_init()
{
    LAPICAddr = (struct LAPIC *)(uintptr_t)MADT->LocalControllerAddress;
    for (uint8_t *ptr = (uint8_t *)(MADT->Entries);
         (uintptr_t)(ptr) < (uintptr_t)(MADT) + MADT->Header.Length;
         ptr += *(ptr + 1))
    {
        switch (*(ptr))
        {
        case 0:
            if (ptr[4] & 1)
            {
                LAPIC[apicsnum.lapic] = (struct LocalAPIC *)ptr;
                trace("%02d-LAPIC\tfound CPU ID:%#x, APIC ID:%#x",
                      CPUCores, LAPIC[apicsnum.lapic]->ACPIProcessorId, LAPIC[apicsnum.lapic]->APICId);
                CPUCores++;
                apicsnum.lapic++;
                apicsnum.lapic++;
            }
            break;
        case 1:
            IOAPIC[apicsnum.ioapic] = (struct MADTIOApic *)ptr;
            trace("%02d-IOAPIC\tfound APICID:%#x at ADDR:%#x",
                  apicsnum.ioapic, IOAPIC[apicsnum.ioapic]->APICID, IOAPIC[apicsnum.ioapic]->addr);
            KernelPageTableManager.MapMemory((void *)(uintptr_t)IOAPIC[apicsnum.ioapic]->addr, (void *)(uintptr_t)IOAPIC[apicsnum.ioapic]->addr, PTFlag::RW | PTFlag::PCD); // Make sure that the address is mapped.
            apicsnum.ioapic++;
            break;
        case 2:
            ISO[apicsnum.iso] = (struct MADTIso *)ptr;
            trace("%02d-ISO\t\tfound IRQ:%#x, BUS:%#x, GSI:%#x, %s/%s",
                  apicsnum.iso, ISO[apicsnum.iso]->IRQSource, ISO[apicsnum.iso]->BuSSource, ISO[apicsnum.iso]->GSI,
                  ISO[apicsnum.iso]->Flags & 0x00000004 ? "Active High" : "Active Low",
                  ISO[apicsnum.iso]->Flags & 0x00000100 ? "Edge Triggered" : "Level Triggered");
            apicsnum.iso++;
            break;
        case 4:
            NMI[apicsnum.nmi] = (struct MADTNmi *)ptr;
            trace("%02d-NMI\t\tfound cpu:%#x, lint:%#x",
                  apicsnum.nmi, NMI[apicsnum.nmi]->processor, NMI[apicsnum.nmi]->lint);
            apicsnum.nmi++;
            break;
        case 5:
            LAPICAddr = (struct LAPIC *)ptr;
            trace("APIC\t\tfound at %#x", LAPICAddr);
            break;
        }
    }
    KernelPageTableManager.MapMemory((void *)LAPICAddr, (void *)LAPICAddr, PTFlag::RW | PTFlag::PCD); // Make sure that the address is mapped.
}

void init_acpi()
{
    trace("initializing ACPI");
    debug("rsdp: %#x", bootparams->rsdp);
    debug("XSDT: %#x", bootparams->rsdp->XSDTAddress);
    debug("RSDT: %#x", bootparams->rsdp->RSDTAddress);
    if (bootparams->rsdp->Revision >= 2 && bootparams->rsdp->XSDTAddress)
    {
        debug("XSDT supported");
        XSDT = (struct ACPIHeader *)(bootparams->rsdp->XSDTAddress);
    }
    else
    {
        debug("RSDT supported");
        XSDT = (struct ACPIHeader *)(uintptr_t)bootparams->rsdp->RSDTAddress;
    }
    if (bootparams->rsdp->Revision >= 2 && bootparams->rsdp->XSDTAddress)
    {
        debug("XSDT supported (boolean)");
        XSDT_supported = true;
    }
    HPET = (struct HPETHeader *)FindTable(XSDT, (char *)"HPET");
    FADT = (struct FADTHeader *)FindTable(XSDT, (char *)"FACP");
    MCFG = (struct MCFGHeader *)FindTable(XSDT, (char *)"MCFG");
    BGRT = (struct BGRTHeader *)FindTable(XSDT, (char *)"BGRT");
    SRAT = (struct SRATHeader *)FindTable(XSDT, (char *)"SRAT");
    TPM2 = (struct TPM2Header *)FindTable(XSDT, (char *)"TPM2");
    TCPA = (struct TCPAHeader *)FindTable(XSDT, (char *)"TCPA");
    WAET = (struct WAETHeader *)FindTable(XSDT, (char *)"WAET");
    MADT = (struct MADTHeader *)FindTable(XSDT, (char *)"APIC");
    FindTable(XSDT, (char *)"BERT");
    FindTable(XSDT, (char *)"CPEP");
    FindTable(XSDT, (char *)"DSDT");
    FindTable(XSDT, (char *)"ECDT");
    FindTable(XSDT, (char *)"EINJ");
    FindTable(XSDT, (char *)"ERST");
    FindTable(XSDT, (char *)"FACS");
    FindTable(XSDT, (char *)"HEST");
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

    outb(FADT->SMI_CommandPort, FADT->AcpiEnable);
    while (!(inw(FADT->PM1aControlBlock) & 1))
        ;
    if (MADT)
        madt_init();
    else
        err("MADT not found or null");
    dsdt_init();
}
