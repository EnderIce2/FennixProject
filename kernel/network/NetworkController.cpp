#include "NetworkController.hpp"

#include <asm.h>

#include "../drivers/network/VirtioNetwork/VirtioNetwork.hpp"
#include "../drivers/network/Intel8254x/Intel8254x.hpp"
#include "../drivers/network/AMDPCNET/AMDPCNET.hpp"
#include "../drivers/network/RTL8139/RTL8139.hpp"
#include "../drivers/network/RTL8169/RTL8169.hpp"
#include "../drivers/network/E1000/E1000.hpp"
#include "../pci.h"

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
                if (mac.Valid())
                {
                    netdbg("RTL-8139[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8139CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
                    Devices[CardIDs++] = card;
                }
                rtl8139CardCount++;
            }

        int rtl8169CardCount = 0;
        foreach (auto card in rtl8169)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (mac.Valid())
                {
                    netdbg("RTL-8169[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8169CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
                    Devices[CardIDs++] = card;
                }
                rtl8169CardCount++;
            }

        int e1000CardCount = 0;
        foreach (auto card in e1000)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (mac.Valid())
                {
                    netdbg("E1000[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", e1000CardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
                    Devices[CardIDs++] = card;
                }
                e1000CardCount++;
            }

        int i8254xCardCount = 0;
        foreach (auto card in i8254x)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (mac.Valid())
                {
                    netdbg("i8254x[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", i8254xCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
                    Devices[CardIDs++] = card;
                }
                i8254xCardCount++;
            }

        int pcnetCardCount = 0;
        foreach (auto card in pcnet)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (mac.Valid())
                {
                    netdbg("PCNET[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", pcnetCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
                    Devices[CardIDs++] = card;
                }
                pcnetCardCount++;
            }

        int virtioCardCount = 0;
        foreach (auto card in virtio)
            if (card)
            {
                MediaAccessControl mac = card->GetMAC();
                if (mac.Valid())
                {
                    netdbg("Virtio[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", virtioCardCount,
                           mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                    card->ID = CardIDs;
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
                rtl8139[rtl8139Count++] = new RTL8139::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8169))
                rtl8169[rtl8169Count++] = new RTL8169::NetworkInterfaceController(PCIData);
        }

        // E1000
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100E))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x153A))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x10EA))
                e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData);
        }

        // 8254x
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x109A))
                i8254x[i8254xCount++] = new Intel8254x::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100F))
                i8254x[i8254xCount++] = new Intel8254x::NetworkInterfaceController(PCIData);
        }

        // 79C970 (AMD PCNET)
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x1022, 0x2000))
                pcnet[pcnetCount++] = new AMDPCNET::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x1022, 0x2001))
                pcnet[pcnetCount++] = new AMDPCNET::NetworkInterfaceController(PCIData);
        }

        // Virtio
        {
            foreach (auto PCIData in PCI::FindPCIDevice(0x1AF4, 0x1000))
                virtio[virtioCount++] = new VirtioNetwork::NetworkInterfaceController(PCIData);

            foreach (auto PCIData in PCI::FindPCIDevice(0x1AF4, 0x1041))
                virtio[virtioCount++] = new VirtioNetwork::NetworkInterfaceController(PCIData);
        }

        TraceCards();
        DbgNetwork();
    }

    NetworkInterface::~NetworkInterface()
    {
        // TODO: warning: deleting object of polymorphic class type ‘X::NetworkInterfaceController’ which has non-virtual destructor might cause undefined behavior
        for (int i = 0; i < CardIDs; i++)
            delete Devices[i];
    }

    void NetworkInterface::StartNetworkStack()
    {
        SysGetCurrentThread()->Info.Priority = 100;
        DeviceInterface *DefaultDevice = nullptr;
        foreach (auto var in Devices)
            if (var)
            {
                DefaultDevice = var;
                break;
            }
        if (!DefaultDevice)
            err("No network device found!");
        else
        {
            NetworkEthernet::Ethernet *eth = new NetworkEthernet::Ethernet(DefaultDevice); // Use the first device found as the ethernet device
            NetworkARP::ARP *arp = new NetworkARP::ARP(eth);
            NetworkIPv4::IPv4 *ipv4 = new NetworkIPv4::IPv4(arp, eth);
            NetworkUDP::UDP *udp = new NetworkUDP::UDP(ipv4, DefaultDevice);
            NetworkUDP::Socket *DHCP_Socket = udp->Connect({.Address = {0xFF, 0xFF, 0xFF, 0xFF}}, 67);
            NetworkDHCP::DHCP *dhcp = new NetworkDHCP::DHCP(DHCP_Socket, DefaultDevice);
            udp->Bind(DHCP_Socket, dhcp);
            dhcp->Request();
            DefaultDevice->IP = InternetProtocol4().FromHex(b32(dhcp->IP.ToHex()));
            ipv4->SubnetworkMaskIP = dhcp->SubnetworkMask;
            ipv4->GatewayIP = dhcp->Gateway;
            arp->Broadcast(dhcp->Gateway);
            trace("IP: %d.%d.%d.%d", dhcp->IP.Address[3], dhcp->IP.Address[2], dhcp->IP.Address[1], dhcp->IP.Address[0]);
            trace("Subnetwork Mask: %d.%d.%d.%d", dhcp->SubnetworkMask.Address[3], dhcp->SubnetworkMask.Address[2], dhcp->SubnetworkMask.Address[1], dhcp->SubnetworkMask.Address[0]);
            trace("Gateway: %d.%d.%d.%d", dhcp->Gateway.Address[3], dhcp->Gateway.Address[2], dhcp->Gateway.Address[1], dhcp->Gateway.Address[0]);
            trace("DNS: %d.%d.%d.%d", dhcp->DomainNameSystem.Address[3], dhcp->DomainNameSystem.Address[2], dhcp->DomainNameSystem.Address[1], dhcp->DomainNameSystem.Address[0]);

            /* TODO: Store everything in an vector and initialize all network cards */
        }

        SysGetCurrentThread()->Info.Priority = 5;

        while (1)
            PAUSE;
    }

    void NetworkInterface::StopNetworkStack()
    {
        fixme("Stop network stack");
    }

    ReadFSFunction(NetRead)
    {
        fixme("Not implemented.");
        return Size;
    }

    WriteFSFunction(NetWrite)
    {
        fixme("Not implemented.");
        return Size;
    }

    FileSystem::FileSystemOpeations netsvcfs = {
        .Name = "Network Service",
        .Read = NetRead,
        .Write = NetWrite};

    void CallStartNetworkStackWrapper() { nimgr->StartNetworkStack(); }

    void NetworkInterface::StartService()
    {
        // TODO: on kill call StopNetworkStack
        netfs = new FileSystem::Network;
        netfs->AddNetworkCard(&netsvcfs, 0666, "default", FileSystem::NodeFlags::FS_PIPE);
        NetSvcProc = SysCreateProcess("Network Service", ELEVATION::System);
        NetSvcThrd = SysCreateThread((PCB *)NetSvcProc, (uint64_t)CallStartNetworkStackWrapper, 0, 0);
    }

    Vector<Events *> RegisteredEvents;

    void NetworkInterface::Send(DeviceInterface *Interface, uint8_t *Data, uint64_t Length)
    {
        void *DataToBeSent = KernelAllocator.RequestPages(Length / PAGE_SIZE + 1);
        memcpy(DataToBeSent, Data, Length);
        Interface->Send(DataToBeSent, Length);
        KernelAllocator.FreePages(DataToBeSent, Length / PAGE_SIZE + 1);
        foreach (auto var in RegisteredEvents)
            var->OnInterfaceSent(Interface, Data, Length);
    }

    void NetworkInterface::Receive(DeviceInterface *Interface, uint8_t *Data, uint64_t Length)
    {
        foreach (auto var in RegisteredEvents)
            var->OnInterfaceReceived(Interface, Data, Length);
    }

    Events::Events(DeviceInterface *Interface) { RegisteredEvents.push_back(this); }
    Events::~Events()
    {
        for (uint64_t i = 0; i < RegisteredEvents.size(); i++)
            if (RegisteredEvents[i] == this)
            {
                RegisteredEvents.remove(i);
                return;
            }
    }
}
