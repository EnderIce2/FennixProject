#include "pci.h"
#include "acpi.h"
#include <heap.h>
#include <string.h>
#include "drivers/serial.h"
#include <debug.h>
#include <bootscreen.h>

using namespace VMM;

Vector<PCI::PCIDeviceHeader *> Devices;

namespace PCI
{
    void EnumerateFunction(uint64_t DeviceAddress, uint64_t Function)
    {
        uint64_t Offset = Function << 12;
        uint64_t FunctionAddress = DeviceAddress + Offset;
        KernelPageTableManager.MapMemory((void *)FunctionAddress, (void *)FunctionAddress, PTFlag::RW);
        PCIDeviceHeader *PCIDeviceHdr = (PCIDeviceHeader *)FunctionAddress;
        if (PCIDeviceHdr->DeviceID == 0)
            return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF)
            return;
        Devices.push_back(PCIDeviceHdr);
        serial_write_text(COM1, (char *)GetVendorName(PCIDeviceHdr->VendorID));
        serial_write_text(COM1, (char *)" / ");
        serial_write_text(COM1, (char *)GetDeviceName(PCIDeviceHdr->VendorID, PCIDeviceHdr->DeviceID));
        serial_write_text(COM1, (char *)" / ");
        serial_write_text(COM1, (char *)DeviceClasses[PCIDeviceHdr->Class]);
        serial_write_text(COM1, (char *)" / ");
        serial_write_text(COM1, (char *)GetSubclassName(PCIDeviceHdr->Class, PCIDeviceHdr->Subclass));
        serial_write_text(COM1, (char *)" / ");
        serial_write_text(COM1, (char *)GetProgIFName(PCIDeviceHdr->Class, PCIDeviceHdr->Subclass, PCIDeviceHdr->ProgIF));
        serial_write_text(COM1, (char *)"\n");
    }

    void EnumerateDevice(uint64_t BusAddress, uint64_t Device)
    {
        uint64_t Offset = Device << 15;
        uint64_t DeviceAddress = BusAddress + Offset;
        KernelPageTableManager.MapMemory((void *)DeviceAddress, (void *)DeviceAddress, PTFlag::RW);
        PCIDeviceHeader *PCIDeviceHdr = (PCIDeviceHeader *)DeviceAddress;
        if (PCIDeviceHdr->DeviceID == 0)
            return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF)
            return;
        for (uint64_t Function = 0; Function < 8; Function++)
            EnumerateFunction(DeviceAddress, Function);
    }

    void EnumerateBus(uint64_t BaseAddress, uint64_t Bus)
    {
        uint64_t Offset = Bus << 20;
        uint64_t BusAddress = BaseAddress + Offset;
        KernelPageTableManager.MapMemory((void *)BusAddress, (void *)BusAddress, PTFlag::RW);
        PCIDeviceHeader *PCIDeviceHdr = (PCIDeviceHeader *)BusAddress;
        if (PCIDeviceHdr->DeviceID == 0)
            return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF)
            return;
        for (uint64_t Device = 0; Device < 32; Device++)
            EnumerateDevice(BusAddress, Device);
    }

    void EnumeratePCI(struct MCFGHeader *MCFG)
    {
        int Entries = ((MCFG->Header.Length) - sizeof(struct MCFGHeader)) / sizeof(struct DeviceConfig);
        for (int t = 0; t < Entries; t++)
        {
            struct DeviceConfig *NewDeviceConfig = (struct DeviceConfig *)((uint64_t)MCFG + sizeof(struct MCFGHeader) + (sizeof(struct DeviceConfig) * t));
            KernelPageTableManager.MapMemory((void *)NewDeviceConfig->BaseAddress, (void *)NewDeviceConfig->BaseAddress, PTFlag::RW | PTFlag::PCD);
            trace("PCI entry %d addr:%016p BUS:%#llx-%#llx", t, NewDeviceConfig->BaseAddress,
                  NewDeviceConfig->StartBus, NewDeviceConfig->EndBus);
            for (uint64_t Bus = NewDeviceConfig->StartBus; Bus < NewDeviceConfig->EndBus; Bus++)
                EnumerateBus(NewDeviceConfig->BaseAddress, Bus);
        }
    }

    Vector<PCIDeviceHeader *> FindPCIDevice(uint8_t Class, uint8_t Subclass, uint8_t ProgIF)
    {
        Vector<PCIDeviceHeader *> DeviceFound;
        for (auto var : Devices)
            if (var->Class == Class && var->Subclass == Subclass && var->ProgIF == ProgIF)
                DeviceFound.push_back(var);
        return DeviceFound;
    }
}

void *FindTable(struct ACPIHeader *ACPIHeader, char *Signature)
{
    for (long unsigned int t = 0; t < ((ACPIHeader->Length - sizeof(struct ACPIHeader)) / (XSDT_supported ? 8 : 4)); t++)
    {
        struct ACPIHeader *SDTHdr = (struct ACPIHeader *)*(uint64_t *)((uint64_t)ACPIHeader + sizeof(struct ACPIHeader) + (t * 8));
        for (int i = 0; i < 4; i++)
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
    // WARN("\t%s not found!", Signature);
    return 0;
}

void init_pci()
{
    debug("%#llx", MCFG);
    if (MCFG != NULL)
    {
        PCI::EnumeratePCI(MCFG);
    }
    else
    {
        err("Error! No PCI support!"); // TODO: add legacy support if MCFG is not available
    }
    BS->IncreaseProgres();
}
