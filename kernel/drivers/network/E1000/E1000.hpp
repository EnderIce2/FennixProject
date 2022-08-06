#pragma once

#include <int.h>

#include "../../../network/NetworkController.hpp"
#include "../../../pci.h"

namespace E1000
{
    class NetworkInterfaceController : public NetworkInterfaceManager::DeviceInterface, public DriverInterrupts::Register
    {
    private:
#define INTEL_VEND 0x8086    // Vendor ID for Intel
#define E1000_DEV 0x100E     // Device ID for the e1000 Qemu, Bochs, and VirtualBox emmulated NICs
#define E1000_I217 0x153A    // Device ID for Intel I217
#define E1000_82577LM 0x10EA // Device ID for Intel 82577LM
#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8

        enum REG
        {
            CTRL = 0x0000,
            STATUS = 0x0008,
            EEPROM = 0x0014,
            CTRL_EXT = 0x0018,
            IMASK = 0x00D0,
            RCTRL = 0x0100,
            RXDESCLO = 0x2800,
            RXDESCHI = 0x2804,
            RXDESCLEN = 0x2808,
            RXDESCHEAD = 0x2810,
            RXDESCTAIL = 0x2818,
            TCTRL = 0x0400,
            TXDESCLO = 0x3800,
            TXDESCHI = 0x3804,
            TXDESCLEN = 0x3808,
            TXDESCHEAD = 0x3810,
            TXDESCTAIL = 0x3818,
            RDTR = 0x2820,
            RXDCTL = 0x3828,
            RADV = 0x282C,
            RSRPD = 0x2C00,
            TIPG = 0x0410
        };

        enum ECTRL
        {
            SLU = 0x40
        };

        enum RTCL
        {
            RDMTS_HALF = (0 << 8),
            RDMTS_QUARTER = (1 << 8),
            RDMTS_EIGHTH = (2 << 8)
        };

        enum RCTL
        {
            EN = (1 << 1),
            SBP = (1 << 2),
            UPE = (1 << 3),
            MPE = (1 << 4),
            LPE = (1 << 5),
            LBM_NONE = (0 << 6),
            LBM_PHY = (3 << 6),
            MO_36 = (0 << 12),
            MO_35 = (1 << 12),
            MO_34 = (2 << 12),
            MO_32 = (3 << 12),
            BAM = (1 << 15),
            VFE = (1 << 18),
            CFIEN = (1 << 19),
            CFI = (1 << 20),
            DPF = (1 << 22),
            PMCF = (1 << 23),
            SECRC = (1 << 26),
            BSIZE_256 = (3 << 16),
            BSIZE_512 = (2 << 16),
            BSIZE_1024 = (1 << 16),
            BSIZE_2048 = (0 << 16),
            BSIZE_4096 = ((3 << 16) | (1 << 25)),
            BSIZE_8192 = ((2 << 16) | (1 << 25)),
            BSIZE_16384 = ((1 << 16) | (1 << 25))
        };

        enum CMD
        {
            EOP = (1 << 0),
            IFCS = (1 << 1),
            IC = (1 << 2),
            RS = (1 << 3),
            RPS = (1 << 4),
            VLE = (1 << 6),
            IDE = (1 << 7)
        };

        enum TCTL
        {
            EN_ = (1 << 1),
            PSP = (1 << 3),
            CT_SHIFT = 4,
            COLD_SHIFT = 12,
            SWXOFF = (1 << 22),
            RTLC = (1 << 24)
        };

        enum TSTA
        {
            DD = (1 << 0),
            EC = (1 << 1),
            LC = (1 << 2)
        };

        enum LSTA
        {
            LSTA_TU = (1 << 3)
        };

        struct RXDescriptor
        {
            volatile uint64_t Address;
            volatile uint16_t Length;
            volatile uint16_t Checksum;
            volatile uint8_t Status;
            volatile uint8_t Errors;
            volatile uint16_t Special;
        } __attribute__((packed));

        struct TXDescriptor
        {
            volatile uint64_t Address;
            volatile uint16_t Length;
            volatile uint8_t cso;
            volatile uint8_t Command;
            volatile uint8_t Status;
            volatile uint8_t css;
            volatile uint16_t Special;
        } __attribute__((packed));

        struct BARData
        {
            uint8_t Type;
            uint16_t IOBase;
            uint32_t MemoryBase;
        };

        InternetProtocol IP;
        uint32_t CurrentPacket;
        BARData BAR;
        bool EEPROMAvailable;
        PCI::PCIDeviceHeader *PCIAddress;

        uint16_t RXCurrent;
        uint16_t TXCurrent;
        RXDescriptor *RXDescriptor[E1000_NUM_RX_DESC];
        TXDescriptor *TXDescriptor[E1000_NUM_TX_DESC];

        void OutCMD(uint16_t Address, uint32_t Value);
        uint32_t InCMD(uint16_t Address);

        void StartLink();

        bool DetectEEPROM();
        uint32_t ReadEEPROM(uint8_t Address);
        void rxinit();
        void txinit();

    public:
        MediaAccessControl GetMAC();
        InternetProtocol GetIP();
        void SetIP(InternetProtocol IP);

        NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID);
        ~NetworkInterfaceController();
        bool Start();

        void Send(void *Data, uint64_t Length);
        void Receive();

        virtual void HandleInterrupt(TrapFrame *regs);
    };
}
