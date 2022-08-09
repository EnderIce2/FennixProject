#pragma once

#include <int.h>

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace Intel8254x
{
    class NetworkInterfaceController : public NetworkInterfaceManager::DeviceInterface, public DriverInterrupts::Register
    {
    private:
        struct BARData
        {
            uint8_t Type;
            uint16_t IOBase;
            uint32_t MemoryBase;
        };

        InternetProtocol4 IP;
        BARData BAR;

    public:
        MediaAccessControl GetMAC();
        InternetProtocol4 GetIP();
        void SetIP(InternetProtocol4 IP);

        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress);
        ~NetworkInterfaceController();

        void Send(uint8_t *Data, uint64_t Length);
        void Receive();

        virtual void HandleInterrupt();
    };
}
