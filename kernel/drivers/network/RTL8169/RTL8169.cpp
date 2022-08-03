#include "RTL8169.hpp"

/* https://wiki.osdev.org/RTL8169 */

namespace RTL8169
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
        if (PCIBaseAddress->VendorID != 0x10EC || PCIBaseAddress->DeviceID != 0x8169)
        {
            netdbg("Not a RTL-8169 network card");
            return;
        }
        netdbg("Found RTL-8169 network card");
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

    void NetworkInterfaceController::RTL8169InterruptHandler()
    {
    }
}
