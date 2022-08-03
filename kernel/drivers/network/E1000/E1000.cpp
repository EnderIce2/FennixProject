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

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID)
    {
        if (PCIBaseAddress->VendorID != 0x8086)
            if (PCIBaseAddress->DeviceID != 0x100E && PCIBaseAddress->DeviceID != 0x153A &&
                PCIBaseAddress->DeviceID != 0x10EA && PCIBaseAddress->DeviceID != 0x109A && PCIBaseAddress->DeviceID != 0x100F)
            {
                netdbg("Not a Intel-Ethernet-i217 network card");
                return;
            }
        netdbg("Found Intel-Ethernet-i217 network card");
        PCIAddress = PCIBaseAddress;

        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;

        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);

        // On osdev.org variant
        // BAR.Type = PCIBAR->getPCIBarType(0);
        // BAR.IOBase = PCIBAR->getPCIBar(PCI_BAR_IO) & ~1;
        // BAR.MemoryBase= PCIBAR->getPCIBar(PCI_BAR_MEM) & ~3;

        // PCIBAR->enablePCIBusMastering();

        this->EEPROMAvailable = false;

        // this->Start();

        // MediaAccessControl mac = GetMAC();
        // netdbg("MAC: %02x:%02x:%02x:%02x:%02x:%02x",
        //        mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::writeCommand(uint16_t p_address, uint32_t p_value)
    {
        if (BAR.Type == 0)
        {
            mmioout32(BAR.MemoryBase + p_address, p_value);
        }
        else
        {
            outportl(BAR.IOBase, p_address);
            outportl(BAR.IOBase + 4, p_value);
        }
    }
    uint32_t NetworkInterfaceController::readCommand(uint16_t p_address)
    {
        if (BAR.Type == 0)
        {
            return mmioin32(BAR.MemoryBase + p_address);
        }
        else
        {
            outportl(BAR.IOBase, p_address);
            return inportl(BAR.IOBase + 4);
        }
    }

    bool NetworkInterfaceController::DetectEEPROM()
    {
        uint32_t val = 0;
        writeCommand(REG::EEPROM, 0x1);

        for (int i = 0; i < 1000 && !this->EEPROMAvailable; i++)
        {
            val = readCommand(REG::EEPROM);
            if (val & 0x10)
                this->EEPROMAvailable = true;
            else
                this->EEPROMAvailable = false;
        }
        return this->EEPROMAvailable;
    }

    uint32_t NetworkInterfaceController::ReadEEPROM(uint8_t addr)
    {
        uint16_t data = 0;
        uint32_t tmp = 0;
        if (this->EEPROMAvailable)
        {
            writeCommand(REG::EEPROM, (1) | ((uint32_t)(addr) << 8));
            while (!((tmp = readCommand(REG::EEPROM)) & (1 << 4)))
                ;
        }
        else
        {
            writeCommand(REG::EEPROM, (1) | ((uint32_t)(addr) << 2));
            while (!((tmp = readCommand(REG::EEPROM)) & (1 << 1)))
                ;
        }
        data = (uint16_t)((tmp >> 16) & 0xFFFF);
        return data;
    }

    void NetworkInterfaceController::rxinit()
    {
        uint8_t *ptr;
        struct RXDescriptor *descs;

        // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
        // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones

        ptr = (uint8_t *)(kmalloc(sizeof(struct RXDescriptor) * E1000_NUM_RX_DESC + 16));

        descs = (struct RXDescriptor *)ptr;
        for (int i = 0; i < E1000_NUM_RX_DESC; i++)
        {
            RXDescriptor[i] = (struct RXDescriptor *)((uint8_t *)descs + i * 16);
            RXDescriptor[i]->Address = (uint64_t)(uint8_t *)(kmalloc(8192 + 16));
            RXDescriptor[i]->Status = 0;
        }

        writeCommand(REG::TXDESCLO, (uint32_t)((uint64_t)ptr >> 32));
        writeCommand(REG::TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

        writeCommand(REG::RXDESCLO, (uint64_t)ptr);
        writeCommand(REG::RXDESCHI, 0);

        writeCommand(REG::RXDESCLEN, E1000_NUM_RX_DESC * 16);

        writeCommand(REG::RXDESCHEAD, 0);
        writeCommand(REG::RXDESCTAIL, E1000_NUM_RX_DESC - 1);
        RXCurrent = 0;
        writeCommand(REG::RCTRL, RCTL::EN | RCTL::SBP | RCTL::UPE | RCTL::MPE | RCTL::LBM_NONE | RTCL::RDMTS_HALF | RCTL::BAM | RCTL::SECRC | RCTL::BSIZE_8192);
    }

    void NetworkInterfaceController::txinit()
    {
        uint8_t *ptr;
        struct TXDescriptor *descs;
        // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
        // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
        ptr = (uint8_t *)(kmalloc(sizeof(struct TXDescriptor) * E1000_NUM_TX_DESC + 16));

        descs = (struct TXDescriptor *)ptr;
        for (int i = 0; i < E1000_NUM_TX_DESC; i++)
        {
            TXDescriptor[i] = (struct TXDescriptor *)((uint8_t *)descs + i * 16);
            TXDescriptor[i]->Address = 0;
            TXDescriptor[i]->Command = 0;
            TXDescriptor[i]->Status = TSTA::DD;
        }

        writeCommand(REG::TXDESCHI, (uint32_t)((uint64_t)ptr >> 32));
        writeCommand(REG::TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

        // now setup total length of descriptors
        writeCommand(REG::TXDESCLEN, E1000_NUM_TX_DESC * 16);

        // setup numbers
        writeCommand(REG::TXDESCHEAD, 0);
        writeCommand(REG::TXDESCTAIL, 0);
        TXCurrent = 0;
        writeCommand(REG::TCTRL, TCTL::EN_ | TCTL::PSP | (15 << TCTL::CT_SHIFT) | (64 << TCTL::COLD_SHIFT) | TCTL::RTLC);

        // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards
        // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
        // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
        writeCommand(REG::TCTRL, 0b0110000000000111111000011111010);
        writeCommand(REG::TIPG, 0x0060200A);
    }

    bool NetworkInterfaceController::Start()
    {
        DetectEEPROM();
        // if (!readMACAddress())
        //     return false;
        // startLink();

        for (int i = 0; i < 0x80; i++)
            writeCommand(0x5200 + i * 4, 0);

        // if (interruptManager->registerInterrupt(IRQ0 + PCIAddress->getIntLine(), this))
        // {

        // Enable interrupts
        writeCommand(REG::IMASK, 0x1F6DC);
        writeCommand(REG::IMASK, 0xff & ~4);
        readCommand(0xc0);

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
        uint8_t old_cur = TXCurrent;
        TXCurrent = (TXCurrent + 1) % E1000_NUM_TX_DESC;
        writeCommand(REG::TXDESCTAIL, TXCurrent);
        while (!(TXDescriptor[old_cur]->Status & 0xff))
            ;
    }

    void NetworkInterfaceController::Receive()
    {
        uint16_t old_cur;
        bool got_packet = false;

        while ((RXDescriptor[RXCurrent]->Status & 0x1))
        {
            got_packet = true;
            uint8_t *buf = (uint8_t *)RXDescriptor[RXCurrent]->Address;
            uint16_t len = RXDescriptor[RXCurrent]->Length;

            // Here you should inject the received packet into your network stack

            RXDescriptor[RXCurrent]->Status = 0;
            old_cur = RXCurrent;
            RXCurrent = (RXCurrent + 1) % E1000_NUM_RX_DESC;
            writeCommand(REG::RXDESCTAIL, old_cur);
        }
    }

    void NetworkInterfaceController::E1000InterruptHandler(TrapFrame *regs)
    {
        // if (regs->getInteruptNumber() == pciConfigHeader->getIntLine() + IRQ0)
        // {
        /* This might be needed here if your handler doesn't clear interrupts from each device and must be done before EOI if using the PIC.
           Without this, the card will spam interrupts as the int-line will stay high. */
        writeCommand(REG::IMASK, 0x1);

        uint32_t status = readCommand(0xc0);
        if (status & 0x04)
        {
            // startLink();
            fixme("Start link");
        }
        else if (status & 0x10)
        {
            // good threshold
            fixme("Good threshold");
        }
        else if (status & 0x80)
        {
            this->Receive();
        }
        // }
    }
}
