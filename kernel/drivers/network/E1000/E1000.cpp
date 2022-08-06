#include "E1000.hpp"

#include <io.h>

/* https://wiki.osdev.org/Intel_Ethernet_i217 */

namespace E1000
{
    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        MediaAccessControl mac;
        if (this->EEPROMAvailable)
        {
            uint32_t temp;
            temp = ReadEEPROM(0);
            mac.Address[0] = temp & 0xff;
            mac.Address[1] = temp >> 8;
            temp = ReadEEPROM(1);
            mac.Address[2] = temp & 0xff;
            mac.Address[3] = temp >> 8;
            temp = ReadEEPROM(2);
            mac.Address[4] = temp & 0xff;
            mac.Address[5] = temp >> 8;
        }
        else
        {
            uint8_t *BaseMac8 = (uint8_t *)(BAR.MemoryBase + 0x5400);
            uint32_t *BaseMac32 = (uint32_t *)(BAR.MemoryBase + 0x5400);
            if (BaseMac32[0] != 0)
                for (int i = 0; i < 6; i++)
                    mac.Address[i] = BaseMac8[i];
            else
            {
                warn("No MAC address found.");
                return MediaAccessControl();
            }
        }
        return mac;
    }

    InternetProtocol NetworkInterfaceController::GetIP() { return IP; }
    void NetworkInterfaceController::SetIP(InternetProtocol IP) { this->IP = IP; }

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID) : DriverInterrupts::Register(((PCI::PCIHeader0 *)PCIBaseAddress)->InterruptLine + IRQ0)
    {
        if (PCIBaseAddress->VendorID != 0x8086 && PCIBaseAddress->DeviceID != 0x100E && PCIBaseAddress->DeviceID != 0x153A && PCIBaseAddress->DeviceID != 0x10EA)
        {
            netdbg("Not a Intel-Ethernet-i217 network card");
            return;
        }
        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        PCIAddress = PCIBaseAddress;

        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;

        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);
        this->EEPROMAvailable = false;
        this->Start();
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::OutCMD(uint16_t Address, uint32_t Value)
    {
        if (BAR.Type == 0)
            mmioout32(BAR.MemoryBase + Address, Value);
        else
        {
            outportl(BAR.IOBase, Address);
            outportl(BAR.IOBase + 4, Value);
        }
    }

    uint32_t NetworkInterfaceController::InCMD(uint16_t Address)
    {
        if (BAR.Type == 0)
            return mmioin32(BAR.MemoryBase + Address);
        else
        {
            outportl(BAR.IOBase, Address);
            return inportl(BAR.IOBase + 0x4);
        }
    }

    bool NetworkInterfaceController::DetectEEPROM()
    {
        OutCMD(REG::EEPROM, 0x1);

        for (int i = 0; i < 1000 && !this->EEPROMAvailable; i++)
            if (InCMD(REG::EEPROM) & 0x10)
                this->EEPROMAvailable = true;
            else
                this->EEPROMAvailable = false;

        return this->EEPROMAvailable;
    }

    uint32_t NetworkInterfaceController::ReadEEPROM(uint8_t Address)
    {
        uint16_t Data = 0;
        uint32_t temp = 0;
        if (this->EEPROMAvailable)
        {
            OutCMD(REG::EEPROM, (1) | ((uint32_t)(Address) << 8));
            while (!((temp = InCMD(REG::EEPROM)) & (1 << 4)))
                ;
        }
        else
        {
            OutCMD(REG::EEPROM, (1) | ((uint32_t)(Address) << 2));
            while (!((temp = InCMD(REG::EEPROM)) & (1 << 1)))
                ;
        }
        Data = (uint16_t)((temp >> 16) & 0xFFFF);
        return Data;
    }

    void NetworkInterfaceController::rxinit()
    {
        uint8_t *Ptr = (uint8_t *)KernelAllocator.RequestPages((sizeof(struct RXDescriptor) * E1000_NUM_RX_DESC + 16) / PAGE_SIZE + 1);
        struct RXDescriptor *Descriptor = (struct RXDescriptor *)Ptr;

        for (int i = 0; i < E1000_NUM_RX_DESC; i++)
        {
            RXDescriptor[i] = (struct RXDescriptor *)((uint8_t *)Descriptor + i * 16);
            RXDescriptor[i]->Address = (uint64_t)(uint8_t *)KernelAllocator.RequestPages((8192 + 16) / PAGE_SIZE + 1);
            RXDescriptor[i]->Status = 0;
        }

        OutCMD(REG::TXDESCLO, (uint32_t)((uint64_t)Ptr >> 32));
        OutCMD(REG::TXDESCHI, (uint32_t)((uint64_t)Ptr & 0xFFFFFFFF));

        OutCMD(REG::RXDESCLO, (uint64_t)Ptr);
        OutCMD(REG::RXDESCHI, 0);

        OutCMD(REG::RXDESCLEN, E1000_NUM_RX_DESC * 16);

        OutCMD(REG::RXDESCHEAD, 0);
        OutCMD(REG::RXDESCTAIL, E1000_NUM_RX_DESC - 1);
        RXCurrent = 0;
        OutCMD(REG::RCTRL, RCTL::EN | RCTL::SBP | RCTL::UPE | RCTL::MPE | RCTL::LBM_NONE | RTCL::RDMTS_HALF | RCTL::BAM | RCTL::SECRC | RCTL::BSIZE_8192);
    }

    void NetworkInterfaceController::txinit()
    {
        uint8_t *Ptr = (uint8_t *)KernelAllocator.RequestPages((sizeof(struct TXDescriptor) * E1000_NUM_RX_DESC + 16) / PAGE_SIZE + 1);
        struct TXDescriptor *Descriptor = (struct TXDescriptor *)Ptr;

        for (int i = 0; i < E1000_NUM_TX_DESC; i++)
        {
            TXDescriptor[i] = (struct TXDescriptor *)((uint8_t *)Descriptor + i * 16);
            TXDescriptor[i]->Address = 0;
            TXDescriptor[i]->Command = 0;
            TXDescriptor[i]->Status = TSTA::DD;
        }

        OutCMD(REG::TXDESCHI, (uint32_t)((uint64_t)Ptr >> 32));
        OutCMD(REG::TXDESCLO, (uint32_t)((uint64_t)Ptr & 0xFFFFFFFF));

        OutCMD(REG::TXDESCLEN, E1000_NUM_TX_DESC * 16);

        OutCMD(REG::TXDESCHEAD, 0);
        OutCMD(REG::TXDESCTAIL, 0);
        TXCurrent = 0;
        OutCMD(REG::TCTRL, TCTL::EN_ | TCTL::PSP | (15 << TCTL::CT_SHIFT) | (64 << TCTL::COLD_SHIFT) | TCTL::RTLC);

        // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards
        // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
        // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
        OutCMD(REG::TCTRL, 0b0110000000000111111000011111010);
        OutCMD(REG::TIPG, 0x0060200A);
    }

    void NetworkInterfaceController::StartLink()
    {
        uint32_t ret = InCMD(REG::CTRL);
        OutCMD(REG::CTRL, ret | ECTRL::SLU);
    }

    bool NetworkInterfaceController::Start()
    {
        DetectEEPROM();
        if (!ValidMAC(this->GetMAC()))
            return false;
        StartLink();

        for (int i = 0; i < 0x80; i++)
            OutCMD(0x5200 + i * 4, 0);

        // if (RegisterInterrupt(this->HandleInterrupt, IRQ0 + ((PCI::PCIHeader0 *)PCIAddress)->InterruptLine, true))
        // {
        OutCMD(REG::IMASK, 0x1F6DC);
        OutCMD(REG::IMASK, 0xff & ~4);
        InCMD(0xc0);

        rxinit();
        txinit();
        netdbg("E1000 Started");
        return true;
        // }
        // else
        // return false;
    }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
        TXDescriptor[TXCurrent]->Address = (uint64_t)Data;
        TXDescriptor[TXCurrent]->Length = Length;
        TXDescriptor[TXCurrent]->Command = CMD::EOP | CMD::IFCS | CMD::RS;
        TXDescriptor[TXCurrent]->Status = 0;
        uint8_t OldTXCurrent = TXCurrent;
        TXCurrent = (TXCurrent + 1) % E1000_NUM_TX_DESC;
        OutCMD(REG::TXDESCTAIL, TXCurrent);
        while (!(TXDescriptor[OldTXCurrent]->Status & 0xff))
            ;
    }

    void NetworkInterfaceController::Receive()
    {
        while ((RXDescriptor[RXCurrent]->Status & 0x1))
        {
            uint8_t *Data = (uint8_t *)RXDescriptor[RXCurrent]->Address;
            uint16_t DataLength = RXDescriptor[RXCurrent]->Length;

            fixme("Received packet of length %d", DataLength);

            // Here you should inject the received packet into your network stack

            RXDescriptor[RXCurrent]->Status = 0;
            uint16_t OldRXCurrent = RXCurrent;
            RXCurrent = (RXCurrent + 1) % E1000_NUM_RX_DESC;
            OutCMD(REG::RXDESCTAIL, OldRXCurrent);
        }
    }

    void NetworkInterfaceController::HandleInterrupt(TrapFrame *regs)
    {
        netdbg("Handle interrupt IRQ%d", regs->int_num - IRQ0);
        OutCMD(REG::IMASK, 0x1);

        uint32_t status = InCMD(0xc0);
        if (status & 0x04)
        {
            StartLink();
        }
        else if (status & 0x10)
        {
            fixme("Good threshold"); // ?????
        }
        else if (status & 0x80)
        {
            this->Receive();
        }
    }
}
