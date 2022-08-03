#include "RTL8139.hpp"

namespace RTL8139
{

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID)
    {
        if (PCIBaseAddress->VendorID != 0x10EC || PCIBaseAddress->DeviceID != 0x8139)
        {
            netdbg("Not a RTL-8139 network card");
            return;
        }
        netdbg("Found RTL-8139 network card");
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
    }

    void NetworkInterfaceController::Receive(void *Data)
    {
    }

}
