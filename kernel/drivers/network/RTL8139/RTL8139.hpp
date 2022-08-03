#pragma once

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace RTL8139
{
    class NetworkInterfaceController : public NetworkInterfaceManager::DeviceInterface
    {
    public:
        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID);
        ~NetworkInterfaceController();
        void Send(void *Data, uint64_t Length);
        void Receive(void *Data);
    };
}
