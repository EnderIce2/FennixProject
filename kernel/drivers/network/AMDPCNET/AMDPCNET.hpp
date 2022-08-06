#pragma once

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace AMDPCNET
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

        char *RXBuffer;
        int TXCurrent;
        InternetProtocol IP;
        uint32_t CurrentPacket;
        BARData BAR;

        void WriteRAP32(uint32_t Value);
        void WriteRAP16(uint16_t Value);
        uint32_t ReadCSR32(uint32_t CSR);
        uint16_t ReadCSR16(uint16_t CSR);
        void WriteCSR32(uint32_t CSR, uint32_t Value);
        void WriteCSR16(uint16_t CSR, uint16_t Value);
        void AMDPCNETInterruptHandler();

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
