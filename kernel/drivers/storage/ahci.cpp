#include "ahci.h"
#include <heap.h>
#include <debug.h>
#include "../disk.h"

using namespace DiskManager;

namespace AHCI
{

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000

    string PortTypeName[] = {"None",
                             "SATA",
                             "SEMB",
                             "PM",
                             "SATAPI"};

    PortType CheckPortType(HBAPort *Port)
    {
        uint32_t SataStatus = Port->SataStatus;
        uint8_t InterfacePowerManagement = (SataStatus >> 8) & 0b111;
        uint8_t DeviceDetection = SataStatus & 0b111;

        if (DeviceDetection != HBA_PORT_DEV_PRESENT)
            return PortType::None;
        if (InterfacePowerManagement != HBA_PORT_IPM_ACTIVE)
            return PortType::None;

        switch (Port->Signature)
        {
        case SATA_SIG_ATAPI:
            return PortType::SATAPI;
        case SATA_SIG_ATA:
            return PortType::SATA;
        case SATA_SIG_PM:
            return PortType::PM;
        case SATA_SIG_SEMB:
            return PortType::SEMB;
        default:
            return PortType::None;
        }
    }

    void AHCIDriver::ProbePorts()
    {
        uint32_t PortsImplemented = ABAR->PortsImplemented;
        for (int i = 0; i < 32; i++)
        {
            if (PortsImplemented & (1 << i))
            {
                PortType portType = CheckPortType(&ABAR->Ports[i]);
                if (portType == PortType::SATA || portType == PortType::SATAPI)
                {
                    trace("%s drive found at port %d", PortTypeName[portType], i);
                    Ports[PortCount] = new Port;
                    Ports[PortCount]->AHCIPortType = portType;
                    Ports[PortCount]->HBAPortPtr = &ABAR->Ports[i];
                    Ports[PortCount]->PortNumber = PortCount;
                    Ports[PortCount]->Buffer = static_cast<uint8_t *>(kmalloc(0x1000));
                    PortCount++;
                }
                else
                {
                    if (portType != PortType::None)
                        warn("Unsupported drive type %s found at port %d", PortTypeName[portType], i);
                }
            }
        }
    }

    void Port::Configure()
    {
        StopCMD();

        void *NewBase = KernelAllocator.RequestPage();
        HBAPortPtr->CommandListBase = (uint32_t)(uint64_t)NewBase;
        HBAPortPtr->CommandListBaseUpper = (uint32_t)((uint64_t)NewBase >> 32);
        memset(reinterpret_cast<void *>(HBAPortPtr->CommandListBase), 0, 1024);

        void *FISBase = KernelAllocator.RequestPage();
        HBAPortPtr->FISBaseAddress = (uint32_t)(uint64_t)FISBase;
        HBAPortPtr->FISBaseAddressUpper = (uint32_t)((uint64_t)FISBase >> 32);
        memset(FISBase, 0, 256);

        HBACommandHeader *CommandHeader = (HBACommandHeader *)((uint64_t)HBAPortPtr->CommandListBase + ((uint64_t)HBAPortPtr->CommandListBaseUpper << 32));
        for (int i = 0; i < 32; i++)
        {
            CommandHeader[i].PRDTLength = 8;
            void *CommandTableAddress = KernelAllocator.RequestPage();
            uint64_t Address = (uint64_t)CommandTableAddress + (i << 8);
            CommandHeader[i].CommandTableBaseAddress = (uint32_t)(uint64_t)Address;
            CommandHeader[i].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)Address >> 32);
            memset(CommandTableAddress, 0, 256);
        }
        StartCMD();
    }

    void Port::StopCMD()
    {
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_ST;
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_FRE;
        while (true)
        {
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_FR)
                continue;
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_CR)
                continue;
            break;
        }
    }

    void Port::StartCMD()
    {
        while (HBAPortPtr->CommandStatus & HBA_PxCMD_CR)
            ;
        HBAPortPtr->CommandStatus |= HBA_PxCMD_FRE;
        HBAPortPtr->CommandStatus |= HBA_PxCMD_ST;
    }

    bool Port::ReadWrite(uint64_t Sector, uint32_t SectorCount, void *Buffer, bool Write)
    {
        if (this->PortNumber == PortType::SATAPI && Write)
        {
            err("SATAPI port does not support write.");
            return false;
        }

        uint32_t SectorL = (uint32_t)Sector;
        uint32_t SectorH = (uint32_t)(Sector >> 32);

        HBAPortPtr->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader *CommandHeader = reinterpret_cast<HBACommandHeader *>(HBAPortPtr->CommandListBase);
        CommandHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
        if (Write)
            CommandHeader->Write = 1;
        else
            CommandHeader->Write = 0;
        CommandHeader->PRDTLength = 1;

        HBACommandTable *CommandTable = reinterpret_cast<HBACommandTable *>(CommandHeader->CommandTableBaseAddress);
        memset(CommandTable, 0, sizeof(HBACommandTable) + (CommandHeader->PRDTLength - 1) * sizeof(HBAPRDTEntry));

        CommandTable->PRDTEntry[0].DataBaseAddress = (uint32_t)(uint64_t)Buffer;
        CommandTable->PRDTEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)Buffer >> 32);
        CommandTable->PRDTEntry[0].ByteCount = (SectorCount << 9) - 1; // 512 bytes per sector
        CommandTable->PRDTEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D *CommandFIS = (FIS_REG_H2D *)(&CommandTable->CommandFIS);

        CommandFIS->FISType = FIS_TYPE_REG_H2D;
        CommandFIS->CommandControl = 1;
        if (Write)
            CommandFIS->Command = ATA_CMD_WRITE_DMA_EX;
        else
            CommandFIS->Command = ATA_CMD_READ_DMA_EX;

        CommandFIS->LBA0 = (uint8_t)SectorL;
        CommandFIS->LBA1 = (uint8_t)(SectorL >> 8);
        CommandFIS->LBA2 = (uint8_t)(SectorL >> 16);
        CommandFIS->LBA3 = (uint8_t)SectorH;
        CommandFIS->LBA4 = (uint8_t)(SectorH >> 8);
        CommandFIS->LBA5 = (uint8_t)(SectorH >> 16);

        CommandFIS->DeviceRegister = 1 << 6; // LBA mode
        CommandFIS->CountLow = SectorCount & 0xFF;
        CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

        uint64_t Spin = 0;

        while ((HBAPortPtr->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && Spin < 1000000)
            Spin++;
        if (Spin == 1000000)
        {
            err("Port not responding.");
            return false;
        }

        HBAPortPtr->CommandIssue = 1;

        Spin = 0;
        int TryCount = 0;

        while (true)
        {
            if (Spin > 100000000)
            {
                err("Port %d not responding. (%d)", this->PortNumber, TryCount);
                Spin = 0;
                TryCount++;
                if (TryCount > 10)
                    return false;
            }
            if ((HBAPortPtr->CommandIssue == 0))
                break;
            Spin++;
            if (HBAPortPtr->InterruptStatus & HBA_PxIS_TFES)
            {
                err("Error reading/writing (%d).", Write);
                return false;
            }
        }

        return true;
    }

    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader *PCIBaseAddress)
    {
        this->PCIBaseAddress = PCIBaseAddress;
        trace("AHCI Driver instance initialized");
        ABAR = reinterpret_cast<HBAMemory *>(((PCI::PCIHeader0 *)PCIBaseAddress)->BAR5);
        KernelPageTableManager.MapMemory((void *)ABAR, (void *)ABAR, PTFlag::RW);
        ProbePorts();

        if (PortCount == 0)
        {
            err("No AHCI ports found.");
            return;
        }

        // AHCIPorts.PortCount = PortCount;
        for (int i = 0; i < PortCount; i++)
        {
            trace("Configuring port %d...", i);
            Ports[i]->Configure();
            // AHCIPorts.Port[i] = new AHCI::Port;
            // AHCIPorts.Port[i] = Port;
            // Port->Buffer = (uint8_t *)KernelAllocator.RequestPage();
            // memset(Port->Buffer, 0, 0x1000);
            // Port->Read(0, 1, Port->Buffer);
            // for (int t = 0; t < 128; t++)
            // {
            //     write_serial(COM1, Port->Buffer[t]);
            // }
            // write_serial(COM1, '\n');
        }
    }

    AHCIDriver::~AHCIDriver()
    {
        warn("Tried to uninitialize AHCI driver!");
    }
}

// uint8_t *AHCIRead(uint64_t sector)
// {
//     AHCI::Port *ports[32];
//     for (int i = 0; i < PORTCOUNT; i++)
//     {
//         AHCI::Port *Port = ports[i];
//         Port->Configure();
//         Port->Buffer = (uint8_t *)KernelAllocator.RequestPage();
//         memset(Port->Buffer, 0, 0x1000);
//         Port->Read(0, 1, Port->Buffer);
//         // write_serial(COM1, Port->Buffer[t]);
//     }

//     // AHCI::Port port;
//     // port.Configure();
//     // port.Buffer = (uint8_t *)KernelAllocator.RequestPage();
//     // memset(port.Buffer, 0, 0x1000);
//     // port.Read(sector, 1, buffer);
//     // return buffer;
// }