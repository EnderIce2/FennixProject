#include "Intel8254x.hpp"

/* https://wiki.osdev.org/Intel_8254x */

namespace Intel8254x
{
    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        return MediaAccessControl();
    }

    InternetProtocol NetworkInterfaceController::GetIP()
    {
        return IP;
    }

    void NetworkInterfaceController::SetIP(InternetProtocol IP)
    {
        this->IP = IP;
    }

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID)
    {
        if (PCIBaseAddress->VendorID != 0x8086 && PCIBaseAddress->DeviceID != 0x109A && PCIBaseAddress->DeviceID != 0x100F)
        {
            netdbg("Not a Intel-Ethernet-i217 network card");
            return;
        }
        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;
        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);
        warn("Driver for %s is not implemented yet!", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
        fixme("NetworkInterfaceController::Send( %p %ld )", Data, Length);
    }

    void NetworkInterfaceController::Receive()
    {
        fixme("NetworkInterfaceController::Receive( )");
    }

    void NetworkInterfaceController::Intel8254xInterruptHandler()
    {
        fixme("NetworkInterfaceController::Intel8254xInterruptHandler( )");
    }
}
