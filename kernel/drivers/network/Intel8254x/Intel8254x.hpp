#pragma once

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace Intel8254x
{
    class NetworkInterfaceController : public NetworkInterfaceManager::DeviceInterface
    {
    private:
        struct BARData
        {
            uint8_t Type;
            uint16_t IOBase;
            uint32_t MemoryBase;
        };

        InternetProtocol IP;
        BARData BAR;
        void Intel8254xInterruptHandler();

    public:
        MediaAccessControl GetMAC();
        InternetProtocol GetIP();
        void SetIP(InternetProtocol IP);
        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID);
        ~NetworkInterfaceController();
        void Send(void *Data, uint64_t Length);
        void Receive();
    };
}
