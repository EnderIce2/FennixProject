#pragma once
#include <types.h>
#include "acpi.h"
#include "kernel.h"

#ifdef __cplusplus
#include <vector.hpp>

namespace PCI
{
    struct PCIDeviceHeader
    {
        uint16_t VendorID;
        uint16_t DeviceID;
        uint16_t Command;
        uint16_t Status;
        uint8_t RevisionID;
        uint8_t ProgIF;
        uint8_t Subclass;
        uint8_t Class;
        uint8_t CacheLineSize;
        uint8_t LatencyTimer;
        uint8_t HeaderType;
        uint8_t BIST;
    };

    struct PCIHeader0
    {
        PCIDeviceHeader Header;
        uint32_t BAR0;
        uint32_t BAR1;
        uint32_t BAR2;
        uint32_t BAR3;
        uint32_t BAR4;
        uint32_t BAR5;
        uint32_t CardbusCISPtr;
        uint16_t SubsystemVendorID;
        uint16_t SubsystemID;
        uint32_t ExpansionROMBaseAddr;
        uint8_t CapabilitiesPtr;
        uint8_t Rsv0;
        uint16_t Rsv1;
        uint32_t Rsv2;
        uint8_t InterruptLine;
        uint8_t InterruptPin;
        uint8_t MinGrant;
        uint8_t MaxLatency;
    };

    void EnumeratePCI(struct MCFGHeader *MCFG);
    void *FindTable(struct ACPIHeader *ACPIHeader, char *Signature);

    extern const char *DeviceClasses[];

    const char *GetVendorName(uint16_t VendorID);
    const char *GetDeviceName(uint16_t VendorID, uint16_t DeviceID);
    const char *GetSubclassName(uint8_t ClassCode, uint8_t SubclassCode);
    const char *GetProgIFName(uint8_t ClassCode, uint8_t SubclassCode, uint8_t ProgIF);
    Vector<PCIDeviceHeader *> FindPCIDevice(uint8_t Class, uint8_t Subclass, uint8_t ProgIF);
}

#else
struct PCIDeviceHeader
{
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgIF;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
};

struct PCIHeader0
{
    struct PCIDeviceHeader Header;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t CardbusCISPtr;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemID;
    uint32_t ExpansionROMBaseAddr;
    uint8_t CapabilitiesPtr;
    uint8_t Rsv0;
    uint16_t Rsv1;
    uint32_t Rsv2;
    uint8_t InterruptLine;
    uint8_t InterruptPin;
    uint8_t MinGrant;
    uint8_t MaxLatency;
};

#endif

EXTERNC void *FindTable(struct ACPIHeader *ACPIHeader, char *Signature);
EXTERNC void init_pci();
