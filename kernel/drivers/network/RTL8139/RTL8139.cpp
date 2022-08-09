#include "RTL8139.hpp"

#include <assert.h>
#include <heap.h>
#include <asm.h>
#include <io.h>

namespace RTL8139
{
    uint8_t TSAD[4] = {0x20, 0x24, 0x28, 0x2C};
    uint8_t TSD[4] = {0x10, 0x14, 0x18, 0x1C};

    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        uint32_t MAC1 = RTLIL(0x0);
        uint16_t MAC2 = RTLIW(0x04);
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
        // Making sure that bus master is enabled
        PCIBaseAddress->Command |= PCI::PCI_COMMAND_MASTER | PCI::PCI_COMMAND_IO | PCI::PCI_COMMAND_MEMORY;
        uint32_t PCIBAR0 = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;
        uint32_t PCIBAR1 = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR1;

        BAR.Type = PCIBAR1 & 1;
        BAR.IOBase = PCIBAR0 & (~3);
        BAR.MemoryBase = PCIBAR1 & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);
        memcpy(this->Name, "RTL-8139 Network Driver", sizeof(this->Name));

        RXBuffer = (uint8_t *)KernelAllocator.RequestPages(2);
        RTLOB(0x52, 0x0);
        RTLOB(0x37, (1 << 4));
        while ((RTLIB(0x37) & (1 << 4)))
            ;
        RTLOL(0x30, static_cast<uint32_t>(reinterpret_cast<uint64_t>(RXBuffer)));
        RTLOW(0x3C, ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
                     (1 << 4) | (1 << 5) | (1 << 6) | (1 << 13) |
                     (1 << 14) | (1 << 15)));
        RTLOL(0x44, ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 7)));
        RTLOB(0x37, 0x0C);
        this->MAC = this->GetMAC();
    }

    NetworkInterfaceController::~NetworkInterfaceController() { KernelAllocator.FreePages(RXBuffer, 2); }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
        RTLOL(TSAD[TXCurrent], static_cast<uint32_t>(reinterpret_cast<uint64_t>(Data)));
        RTLOL(TSD[TXCurrent++], Length);
        if (TXCurrent > 3)
            TXCurrent = 0;
    }

    void NetworkInterfaceController::Receive()
    {
        uint16_t *Data = (uint16_t *)(RXBuffer + CurrentPacket);
        uint16_t DataLength = *(Data + 1);
        Data = Data + 2;
        nimgr->Receive(this, (uint8_t *)Data, DataLength);
        CurrentPacket = (CurrentPacket + DataLength + 4 + 3) & (~3);
        if (CurrentPacket > 8192)
            CurrentPacket -= 8192;
        RTLOW(0x38, CurrentPacket - 0x10);
    }

    void NetworkInterfaceController::HandleInterrupt(TrapFrame *regs)
    {
        uint16_t status = RTLIW(0x3e);

        char *dbgmsg = (char *)"ERROR";

        if (status & (1 << 2)) // TOK
            dbgmsg = (char *)"I=>O";
        else if (status & (1 << 0)) // ROK
        {
            dbgmsg = (char *)"I<=O";
            this->Receive();
        }
        else
        {
            netdbg("Unknown status %#lx", status);
        }
        netdbg("IRQ%d: [%s]", regs->int_num - IRQ0, dbgmsg);
        RTLOW(0x3E, (1 << 0) | (1 << 2));
    }
}
