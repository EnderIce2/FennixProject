#include "RTL8169.hpp"

/* https://wiki.osdev.org/RTL8169 */

namespace RTL8169
{
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
        if (PCIBaseAddress->VendorID != 0x10EC && PCIBaseAddress->DeviceID != 0x8169)
        {
            netdbg("Not a RTL-8169 network card");
            return;
        }
        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        memcpy(this->Name, "RTL-8169 Network Driver", sizeof(this->Name));

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
