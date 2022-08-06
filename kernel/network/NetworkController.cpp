#include "NetworkController.hpp"

#include "../drivers/network/VirtioNetwork/VirtioNetwork.hpp"
#include "../drivers/network/Intel8254x/Intel8254x.hpp"
#include "../drivers/network/AMDPCNET/AMDPCNET.hpp"
#include "../drivers/network/RTL8139/RTL8139.hpp"
#include "../drivers/network/RTL8169/RTL8169.hpp"
#include "../drivers/network/E1000/E1000.hpp"
#include "../pci.h"

// TODO: warning: deleting object of polymorphic class type ‘X::NetworkInterfaceController’ which has non-virtual destructor might cause undefined behavior

NetworkInterfaceManager::NetworkInterface *nimgr = nullptr;

int rtl8139Count = 0;
RTL8139::NetworkInterfaceController *rtl8139[8];

int rtl8169Count = 0;
RTL8169::NetworkInterfaceController *rtl8169[8];

int e1000Count = 0;
E1000::NetworkInterfaceController *e1000[8];

int i8254xCount = 0;
Intel8254x::NetworkInterfaceController *i8254x[8];

int pcnetCount = 0;
AMDPCNET::NetworkInterfaceController *pcnet[8];

int virtioCount = 0;
VirtioNetwork::NetworkInterfaceController *virtio[8];

namespace NetworkInterfaceManager
{
    void NetworkInterface::TraceCards()
    {
        int rtl8139CardCount = 0;
        foreach (auto card in rtl8139)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("RTL-8139[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8139CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                rtl8139CardCount++;
            }

        int rtl8169CardCount = 0;
        foreach (auto card in rtl8169)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("RTL-8169[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8169CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                rtl8169CardCount++;
            }

        int e1000CardCount = 0;
        foreach (auto card in e1000)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("E1000[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", e1000CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                e1000CardCount++;
            }

        int i8254xCardCount = 0;
        foreach (auto card in i8254x)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("i8254x[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", i8254xCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                i8254xCardCount++;
            }

        int pcnetCardCount = 0;
        foreach (auto card in pcnet)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("PCNET[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", pcnetCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                pcnetCardCount++;
            }

        int virtioCardCount = 0;
        foreach (auto card in virtio)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (ValidMAC(mac))
                {
                    netdbg("Virtio[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", virtioCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    Devices[CardIDs++] = card;
                }
                virtioCardCount++;
            }
    }

    NetworkInterface::NetworkInterface()
    {
        // RTL-81**
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8139))
                rtl8139[rtl8139Count++] = new RTL8139::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8169))
                rtl8169[rtl8169Count++] = new RTL8169::NetworkInterfaceController(PCIData, CardIDs++);
        }

        // E1000
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100E))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x153A))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x10EA))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
        }

        // 8254x
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x109A))
                i8254x[i8254xCount++] = new Intel8254x::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100F))
                i8254x[i8254xCount++] = new Intel8254x::NetworkInterfaceController(PCIData, CardIDs++);
        }

        // 79C970 (AMD PCNET)
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x1022, 0x2000))
                pcnet[pcnetCount++] = new AMDPCNET::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x1022, 0x2001))
                pcnet[pcnetCount++] = new AMDPCNET::NetworkInterfaceController(PCIData, CardIDs++);
        }

        // Virtio
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x1AF4, 0x1000))
                virtio[virtioCount++] = new VirtioNetwork::NetworkInterfaceController(PCIData, CardIDs++);

            foreach (auto PCIData in PCI::FindPCIDevice(0x1AF4, 0x1041))
                virtio[virtioCount++] = new VirtioNetwork::NetworkInterfaceController(PCIData, CardIDs++);
        }

        TraceCards();
    }

    NetworkInterface::~NetworkInterface()
    {
        for (int i = 0; i < CardIDs; i++)
            delete Devices[i];
    }

    void NetworkInterface::Scan()
    {
        fixme("Scan for network interfaces");
    }

    void NetworkInterface::Send(DeviceInterface Interface, void *Data, uint64_t Length) { Interface.Send(Data, Length); }
    void NetworkInterface::Receive(DeviceInterface Interface, void *Data, uint64_t Length) { Interface.Receive(); }
}
