#include "RTL8139.hpp"

#include <assert.h>
#include <heap.h>
#include <asm.h>
#include <io.h>

namespace RTL8139
{
    /* http://realtek.info/pdf/rtl8139cp.pdf */
    uint8_t TSAD[4] = {0x20, 0x24, 0x28, 0x2C};
    uint8_t TSD[4] = {0x10, 0x14, 0x18, 0x1C};

    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        uint32_t MAC1 = inportl(BAR.IOBase + 0x00);
        uint16_t MAC2 = inportw(BAR.IOBase + 0x04);
        MediaAccessControl mac = {
            mac.Address[0] = MAC1,
            mac.Address[1] = (MAC1 >> 8),
            mac.Address[2] = (MAC1 >> 16),
            mac.Address[3] = (MAC1 >> 24),
            mac.Address[4] = MAC2,
            mac.Address[5] = (MAC2 >> 8)};
        return mac;
    }

    InternetProtocol4 NetworkInterfaceController::GetIP() { return IP; }

    void NetworkInterfaceController::SetIP(InternetProtocol4 IP)
    {
        fixme("NetworkInterfaceController::SetIP( %d.%d.%d.%d )", IP.Address[0], IP.Address[1], IP.Address[2], IP.Address[3]);
    }

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress) : DriverInterrupts::Register(((PCI::PCIHeader0 *)PCIBaseAddress)->InterruptLine + IRQ0)
    {
        if (PCIBaseAddress->VendorID != 0x10EC && PCIBaseAddress->DeviceID != 0x8139)
        {
            netdbg("Not a RTL-8139 network card");
            return;
        }
        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;
        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);
        memcpy(this->Name, "RTL-8139 Network Driver", sizeof(this->Name));

        // https://wiki.osdev.org/RTL8139
        outportb(BAR.IOBase + 0x52, 0x0);
        outportb(BAR.IOBase + 0x37, 0x10);
        while ((inb(BAR.IOBase + 0x37) & 0x10) != 0)
            PAUSE;

        RXBuffer = (char *)KernelAllocator.RequestPages(2);
        assert(!((uint64_t)RXBuffer > 0xffffffff));
        memset(RXBuffer, 0x0, 8192 + 16 + 1500);

        outportl(BAR.IOBase + 0x30, (uintptr_t)RXBuffer);
        outportw(BAR.IOBase + 0x3C, 0x0005);
        outportl(BAR.IOBase + 0x44, 0xf | (1 << 7));
        outportb(BAR.IOBase + 0x37, 0x0C);

        this->MAC = this->GetMAC();
    }

    NetworkInterfaceController::~NetworkInterfaceController() { KernelAllocator.FreePages(RXBuffer, 2); }

    void NetworkInterfaceController::Send(uint8_t *Data, uint64_t Length)
    {
        outportl(BAR.IOBase + TSAD[TXCurrent], (uint32_t)(uint64_t)Data);
        outportl(BAR.IOBase + TSD[TXCurrent++], Length);
        if (TXCurrent > 3)
            TXCurrent = 0;
        netdbg("Sent %d bytes", Length);
    }

    void NetworkInterfaceController::Receive()
    {
        uint32_t *Data = (uint32_t *)(RXBuffer + CurrentPacket);
        uint32_t DataLength = *(Data + 1);
        Data = Data + 2;

        nimgr->Receive(this, (uint8_t *)Data, DataLength);

        CurrentPacket = (CurrentPacket + DataLength + 4 + 3) & (~3);

        if (CurrentPacket > 8192)
            CurrentPacket -= 8192;

        outportw(BAR.IOBase + 0x38, CurrentPacket - 0x10);
    }

    void NetworkInterfaceController::HandleInterrupt()
    {
        uint16_t status = inportw(BAR.IOBase + 0x3e);
        outportw(BAR.IOBase + 0x3E, 0x5);

        if (status & (1 << 2)) // TOK
            netdbg("Packet sent");

        if (status & (1 << 0)) // ROK
        {
            netdbg("Received packet");
            this->Receive();
        }
    }
}
