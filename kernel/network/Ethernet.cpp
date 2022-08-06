#include "NetworkController.hpp"

#include <debug.h>
#include <heap.h>

namespace NetworkEthernet
{
    Ethernet::Ethernet(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }

    Ethernet::~Ethernet() {}

    void Ethernet::Send(void *Data, int Length, SendInfo Info)
    {
        uint64_t PacketLength = sizeof(EthernetPacket) + Length;
        EthernetPacket *Packet = (EthernetPacket *)kmalloc(PacketLength);
        Packet->Header = {
            .DestinationMAC = Info.MAC,
            .SourceMAC = Interface->MAC,
            .Type = Info.Type,
        };
        memcpy(Packet->Data, Data, Length);
        Interface->Send((void *)Packet, PacketLength);
        kfree(Packet);
        netdbg("ETH: Sent %d bytes to %#x:%#x:%#x:%#x:%#x", Length, Info.MAC.Address[0], Info.MAC.Address[1], Info.MAC.Address[2], Info.MAC.Address[3], Info.MAC.Address[4], Info.MAC.Address[5]);
    }

    void Ethernet::Receive(void *Data)
    {
        EthernetPacket *Packet = (EthernetPacket *)Data;

        if (!CompareMAC(Packet->Header.DestinationMAC, Interface->MAC) &&
            !CompareMAC(Packet->Header.DestinationMAC, {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}))
        {
            netdbg("ETH: Received data from %#x:%#x:%#x:%#x:%#x:%#x", Packet->Header.SourceMAC.Address[0], Packet->Header.SourceMAC.Address[1], Packet->Header.SourceMAC.Address[2], Packet->Header.SourceMAC.Address[3], Packet->Header.SourceMAC.Address[4], Packet->Header.SourceMAC.Address[5]);
            switch (Packet->Header.Type)
            {
            case ETYPE_IPV4:
                // TODO: NetworkIPv4::IPv4(Interface).Receive(Packet->Data);
                fixme("IPv4 not implemented");
                break;
            case ETYPE_ARP:
                NetworkARP::ARP(Interface).Receive(Packet->Data);
                break;
            case ETYPE_RARP:
                /* TODO: https://en.wikipedia.org/wiki/Reverse_Address_Resolution_Protocol */
                // NetworkRARP::RARP(Interface).Receive(Packet->Data);
                fixme("RARP not implemented");
                break;
            case ETYPE_IPV6:
                // TODO: NetworkIPv6::IPv6(Interface).Receive(Packet->Data);
                fixme("IPv6 not implemented");
                break;
            default:
                warn("ETH: Unknown packet type %#lx", Packet->Header.Type);
                break;
            }
        }
        else
        {
            warn("ETH: Invalid destination MAC %#x:%#x:%#x:%#x:%#x:%#x", Packet->Header.DestinationMAC.Address[0], Packet->Header.DestinationMAC.Address[1], Packet->Header.DestinationMAC.Address[2], Packet->Header.DestinationMAC.Address[3], Packet->Header.DestinationMAC.Address[4], Packet->Header.DestinationMAC.Address[5]);
        }
    }
}
