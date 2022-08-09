#pragma once

#include <int.h>

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace AMDPCNET
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

        void WriteRAP32(uint32_t Value);
        void WriteRAP16(uint16_t Value);
        uint32_t ReadCSR32(uint32_t CSR);
        uint16_t ReadCSR16(uint16_t CSR);
        void WriteCSR32(uint32_t CSR, uint32_t Value);
        void WriteCSR16(uint16_t CSR, uint16_t Value);

    public:
        MediaAccessControl GetMAC();
        InternetProtocol4 GetIP();
        void SetIP(InternetProtocol4 IP);

        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress);
        ~NetworkInterfaceController();

        void Send(uint8_t *Data, uint64_t Length);
        void Receive();

        void HandleInterrupt();
    };
}
