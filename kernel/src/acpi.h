#pragma once
#include <types.h>
#include <interrupts.h>


#define ACPI_TIMER 0x0001
#define ACPI_BUSMASTER 0x0010
#define ACPI_GLOBAL 0x0020
#define ACPI_POWER_BUTTON 0x0100
#define ACPI_SLEEP_BUTTON 0x0200
#define ACPI_RTC_ALARM 0x0400
#define ACPI_PCIE_WAKE 0x4000
#define ACPI_WAKE 0x8000

START_EXTERNC

struct ACPIHeader
{
    unsigned char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

struct GenericAddressStructure
{
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} __attribute__((packed));

struct MCFGHeader
{
    struct ACPIHeader Header;
    uint64_t Reserved;
} __attribute__((packed));

struct FADTHeader
{
    struct ACPIHeader header;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
    uint8_t Reserved;
    uint8_t PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t PM1EventLength;
    uint8_t PM1ControlLength;
    uint8_t PM2ControlLength;
    uint8_t PMTimerLength;
    uint8_t GPE0Length;
    uint8_t GPE1Length;
    uint8_t GPE1Base;
    uint8_t CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t DutyOffset;
    uint8_t DutyWidth;
    uint8_t DayAlarm;
    uint8_t MonthAlarm;
    uint8_t Century;
    uint16_t BootArchitectureFlags;
    uint8_t Reserved2;
    uint32_t Flags;
    struct GenericAddressStructure ResetReg;
    uint8_t ResetValue;
    uint8_t Reserved3[3];
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;
    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
} __attribute__((packed));

struct HPETHeader
{
    struct ACPIHeader header;
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    struct GenericAddressStructure address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__((packed));

struct DeviceConfig
{
    uint64_t BaseAddress;
    uint16_t PCISegGroup;
    uint8_t StartBus;
    uint8_t EndBus;
    uint32_t Reserved;
} __attribute__((packed));

struct MADTHeader
{
    struct ACPIHeader Header;
    uint32_t LocalControllerAddress;
    uint32_t Flags;
    char Entries[];
} __attribute__((packed));

struct APICHeader
{
    uint8_t Type;
    uint8_t Length;
} __attribute__((packed));

struct MADTIso
{
    struct APICHeader Header;
    uint8_t BuSSource;
    uint8_t IRQSource;
    uint32_t GSI;
    uint16_t Flags;
} __attribute__((packed));

// https://wiki.osdev.org/BGRT
struct BGRTHeader
{
    struct ACPIHeader Header;
    uint16_t Version;
    uint8_t Status;
    uint8_t ImageType;
    uint64_t ImageAddress;
    uint32_t ImageOffsetX;
    uint32_t ImageOffsetY;
};

struct SRATHeader
{
    struct ACPIHeader Header;
    uint32_t TableRevision; // Must be value 1
    uint64_t Reserved;      // Reserved, must be zero
};

struct TPM2Header
{
    struct ACPIHeader Header;
    uint32_t Flags;
    uint64_t ControlAddress;
    uint32_t StartMethod;
};

struct TCPAHeader
{
    struct ACPIHeader Header;
    uint16_t Reserved;
    uint32_t MaxLogLength;
    uint64_t LogAddress;
};

struct WAETHeader
{
    struct ACPIHeader Header;
    uint32_t Flags;
};

struct MADTNmi
{
    struct APICHeader Header;
    uint8_t processor;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed));

typedef struct _APICInterruptOverride
{
    struct APICHeader Header;
    uint8_t Bus;
    uint8_t Source;
    uint32_t Interrupt;
    uint16_t Flags;
} __attribute__((packed)) APICInterruptOverride;

struct IOAPIC
{
    struct APICHeader header;
    uint8_t IOAPICId;
    uint8_t Reserved;
    uint32_t IOAPICAddress;
    uint32_t GlobalSystemInterruptBase;
} __attribute__((packed));

struct MADTIOApic
{
    struct APICHeader Header;
    uint8_t APICID;
    uint8_t reserved;
    uint32_t addr;
    uint32_t gsib;
} __attribute__((packed));

struct LocalAPIC
{
    struct APICHeader Header;
    uint8_t ACPIProcessorId;
    uint8_t APICId;
    uint32_t Flags;
} __attribute__((packed));

struct LAPIC
{
    uint8_t id;
    uintptr_t PhysicalAddress;
    void *VirtualAddress;
};

typedef union IOAPIC_VERSION
{
    struct
    {
        uint32_t version : 8;
        uint32_t _reserved0 : 8;
        uint32_t maximum_redirection_entry : 8;
        uint32_t _reserved1 : 8;
    };
    uint32_t raw;
} IOAPIC_VERSION;

typedef struct _apics
{
    int ioapic;
    int iso;
    int nmi;
    int lapic;
} apics;

extern struct ACPIHeader *XSDT;
extern struct MCFGHeader *MCFG;
extern struct HPETHeader *HPET;
extern struct FADTHeader *FADT;
extern struct BGRTHeader *BGRT;
extern struct SRATHeader *SRAT;
extern struct TPM2Header *TPM2;
extern struct TCPAHeader *TCPA;
extern struct WAETHeader *WAET;
extern struct MADTHeader *MADT;
extern bool XSDT_supported;

#define MADT_MAX_SIZE 1024

extern struct MADTIOApic *IOAPIC[];
extern struct MADTIso *ISO[];
extern struct MADTNmi *NMI[];
extern struct LocalAPIC *LAPIC[];
extern struct LAPIC *LAPICAddr;
extern apics apicsnum;
extern uint8_t CPUCores;

void shutdown();
void reboot();
void RegisterSCIEvents();
void init_acpi();
void SCIHandler(REGISTERS *regs);

END_EXTERNC