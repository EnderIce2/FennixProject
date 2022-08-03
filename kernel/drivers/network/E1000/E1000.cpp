#include "E1000.hpp"

/* https://wiki.osdev.org/Intel_Ethernet_i217 */

namespace E1000
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
        if (PCIBaseAddress->VendorID != 0x8086)
            if (PCIBaseAddress->DeviceID != 0x100E && PCIBaseAddress->DeviceID != 0x153A &&
                PCIBaseAddress->DeviceID != 0x10EA && PCIBaseAddress->DeviceID != 0x109A && PCIBaseAddress->DeviceID != 0x100F)
            {
                netdbg("Not a Intel-Ethernet-i217 network card");
                return;
            }
        netdbg("Found Intel-Ethernet-i217 network card");
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
    }

    void NetworkInterfaceController::Receive()
    {
    }

    void NetworkInterfaceController::E1000InterruptHandler()
    {
    }
}
