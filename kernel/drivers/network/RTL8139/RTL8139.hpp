#pragma once

#include <int.h>

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace RTL8139
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
        InternetProtocol IP;
        uint32_t CurrentPacket;
        BARData BAR;

    public:
        MediaAccessControl GetMAC();
        InternetProtocol GetIP();
        void SetIP(InternetProtocol IP);

        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID);
        ~NetworkInterfaceController();

        void Send(void *Data, uint64_t Length);
        void Receive();

        virtual void HandleInterrupt();
    };
}
