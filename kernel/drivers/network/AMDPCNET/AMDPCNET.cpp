#include "AMDPCNET.hpp"

#include <io.h>

/* https://wiki.osdev.org/AMD_PCNET */

namespace AMDPCNET
{
    void NetworkInterfaceController::WriteRAP32(uint32_t Value)
    {
        outportl(BAR.IOBase + 0x14, Value);
    }

    void NetworkInterfaceController::WriteRAP16(uint16_t Value)
    {
        outportw(BAR.IOBase + 0x12, Value);
    }

    uint32_t NetworkInterfaceController::ReadCSR32(uint32_t CSR)
    {
        WriteRAP32(CSR);
        return inportl(BAR.IOBase + 0x10);
    }

    uint16_t NetworkInterfaceController::ReadCSR16(uint16_t CSR)
    {
        WriteRAP32(CSR);
        return inportw(BAR.IOBase + 0x10);
    }

    void NetworkInterfaceController::WriteCSR32(uint32_t CSR, uint32_t Value)
    {
        WriteRAP32(CSR);
        outportl(BAR.IOBase + 0x10, Value);
    }

    void NetworkInterfaceController::WriteCSR16(uint16_t CSR, uint16_t Value)
    {
        WriteRAP16(CSR);
        outportw(BAR.IOBase + 0x10, Value);
    }

    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        return MediaAccessControl();
    }

    InternetProtocol4 NetworkInterfaceController::GetIP()
    {
        return IP;
    }

    void NetworkInterfaceController::SetIP(InternetProtocol4 IP)
    {
        this->IP = IP;
    }

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress) : DriverInterrupts::Register(((PCI::PCIHeader0 *)PCIBaseAddress)->InterruptLine + IRQ0)
    {
        if (PCIBaseAddress->VendorID != 0x1022 && PCIBaseAddress->DeviceID != 0x2000 && PCIBaseAddress->DeviceID != 0x2001)
        {
            netdbg("Not a AMD PCNET network card");
            return;
        }
        if (PCIBaseAddress->DeviceID == 0x2001)
            warn("This AMD PCNET is not supported. (Maybe it works but there is no guarantee)");

        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        memcpy(this->Name, "AMD PCNET Network Driver", sizeof(this->Name));

        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;
        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);

        this->MAC = this->GetMAC();
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::Send(uint8_t *Data, uint64_t Length)
    {
    }

    void NetworkInterfaceController::Receive()
    {
    }

    void NetworkInterfaceController::HandleInterrupt()
    {
    }
}
