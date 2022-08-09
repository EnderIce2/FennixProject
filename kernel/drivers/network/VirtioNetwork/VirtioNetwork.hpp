#pragma once

#include <int.h>

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace VirtioNetwork
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

        char *RXBuffer;
        int TXCurrent;
        InternetProtocol4 IP;
        uint32_t CurrentPacket;
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
