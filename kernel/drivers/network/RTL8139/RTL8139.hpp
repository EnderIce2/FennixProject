#pragma once

#include <int.h>
#include <io.h>

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

        uint8_t *RXBuffer;
        int TXCurrent;
        InternetProtocol4 IP;
        uint32_t CurrentPacket;
        BARData BAR;

        void RTLOB(uint16_t Address, uint8_t Value)
        {
            if (this->BAR.Type == 0)
                mmoutb(reinterpret_cast<void *>(this->BAR.MemoryBase + Address), Value);
            else
                outportb(this->BAR.IOBase + Address, Value);
        }

        void RTLOW(uint16_t Address, uint16_t Value)
        {
            if (this->BAR.Type == 0)
                mmoutw(reinterpret_cast<void *>(this->BAR.MemoryBase + Address), Value);
            else
                outportw(this->BAR.IOBase + Address, Value);
        }

        void RTLOL(uint16_t Address, uint32_t Value)
        {
            if (this->BAR.Type == 0)
                mmoutl(reinterpret_cast<void *>(this->BAR.MemoryBase + Address), Value);
            else
                outportl(this->BAR.IOBase + Address, Value);
        }

        uint8_t RTLIB(uint16_t Address)
        {
            if (this->BAR.Type == 0)
                return mminb(reinterpret_cast<void *>(this->BAR.MemoryBase + Address));
            else
                return inportb(this->BAR.IOBase + Address);
        }

        uint16_t RTLIW(uint16_t Address)
        {
            if (this->BAR.Type == 0)
                return mminw(reinterpret_cast<void *>(this->BAR.MemoryBase + Address));
            else
                return inportw(this->BAR.IOBase + Address);
        }

        uint32_t RTLIL(uint16_t Address)
        {
            if (this->BAR.Type == 0)
                return mminl(reinterpret_cast<void *>(this->BAR.MemoryBase + Address));
            else
                return inportl(this->BAR.IOBase + Address);
        }

    public:
        MediaAccessControl GetMAC();
        InternetProtocol4 GetIP();
        void SetIP(InternetProtocol4 IP);

        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress);
        ~NetworkInterfaceController();

        void Send(void *Data, uint64_t Length);
        void Receive();

        virtual void HandleInterrupt(TrapFrame *regs);
    };
}
