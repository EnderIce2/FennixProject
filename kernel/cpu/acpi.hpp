#pragma once
#include <types.h>
#include <interrupts.h>
#include <vector.hpp>

namespace ACPI
{
    class ACPI
    {
    private:
        /* data */
    public:
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

        struct HPETHeader
        {
            ACPIHeader header;
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

        struct FADTHeader
        {
            ACPIHeader header;
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

        struct BGRTHeader
        {
            ACPIHeader Header;
            uint16_t Version;
            uint8_t Status;
            uint8_t ImageType;
            uint64_t ImageAddress;
            uint32_t ImageOffsetX;
            uint32_t ImageOffsetY;
        };

        struct SRATHeader
        {
            ACPIHeader Header;
            uint32_t TableRevision; // Must be value 1
            uint64_t Reserved;      // Reserved, must be zero
        };

        struct TPM2Header
        {
            ACPIHeader Header;
            uint32_t Flags;
            uint64_t ControlAddress;
            uint32_t StartMethod;
        };

        struct TCPAHeader
        {
            ACPIHeader Header;
            uint16_t Reserved;
            uint32_t MaxLogLength;
            uint64_t LogAddress;
        };

        struct WAETHeader
        {
            ACPIHeader Header;
            uint32_t Flags;
        };

        struct MADTHeader
        {
            ACPIHeader Header;
            uint32_t LocalControllerAddress;
            uint32_t Flags;
            char Entries[];
        } __attribute__((packed));

        ACPIHeader *XSDT = nullptr;
        MCFGHeader *MCFG = nullptr;
        HPETHeader *HPET = nullptr;
        FADTHeader *FADT = nullptr;
        BGRTHeader *BGRT = nullptr;
        SRATHeader *SRAT = nullptr;
        TPM2Header *TPM2 = nullptr;
        TCPAHeader *TCPA = nullptr;
        WAETHeader *WAET = nullptr;
        MADTHeader *MADT = nullptr;
        bool XSDTSupported = false;

        void *FindTable(ACPIHeader *ACPIHeader, char *Signature);
        void SearchTables(ACPIHeader *Header);
        ACPI();
        ~ACPI();
    };

    class MADT
    {
    private:
        /* data */
    public:
        struct APICHeader
        {
            uint8_t Type;
            uint8_t Length;
        } __attribute__((packed));

        struct MADTIOApic
        {
            struct APICHeader Header;
            uint8_t APICID;
            uint8_t reserved;
            uint32_t addr;
            uint32_t gsib;
        } __attribute__((packed));

        struct MADTIso
        {
            struct APICHeader Header;
            uint8_t BuSSource;
            uint8_t IRQSource;
            uint32_t GSI;
            uint16_t Flags;
        } __attribute__((packed));

        struct MADTNmi
        {
            struct APICHeader Header;
            uint8_t processor;
            uint16_t flags;
            uint8_t lint;
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

        Vector<MADTIOApic *> ioapic;
        Vector<MADTIso *> iso;
        Vector<MADTNmi *> nmi;
        Vector<LocalAPIC *> lapic;
        struct LAPIC *LAPICAddr;
        uint8_t CPUCores;

        MADT();
        ~MADT();
    };

    class DSDT
    {
    private:
        uint32_t SMI_CMD = 0;
        uint8_t ACPI_ENABLE = 0;
        uint8_t ACPI_DISABLE = 0;
        uint32_t PM1a_CNT = 0;
        uint32_t PM1b_CNT = 0;
        uint16_t SLP_TYPa = 0;
        uint16_t SLP_TYPb = 0;
        uint16_t SLP_EN = 0;
        uint16_t SCI_EN = 0;
        uint8_t PM1_CNT_LEN = 0;

    public:
        bool ACPIShutdownSupported = false;

        void RegisterSCIEvents();
        void SetSCIevent(uint16_t value);
        uint16_t GetSCIevent();

        void reboot();
        void shutdown();

        void InitSCI();
        DSDT();
        ~DSDT();
    };
}

extern ACPI::ACPI *acpi;
extern ACPI::MADT *madt;
extern ACPI::DSDT *dsdt;
