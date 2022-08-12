#include "NetworkController.hpp"

#include <heap.h>

namespace NetworkIPv4
{
    IPv4::IPv4(NetworkARP::ARP *ARP, NetworkEthernet::Ethernet *Ethernet) : NetworkEthernet::EthernetEvents(NetworkEthernet::TYPE_IPV4)
    {
        netdbg("IPv4: Initializing.");
        this->ARP = ARP;
        this->Ethernet = Ethernet;
    }

    IPv4::~IPv4()
    {
    }

    void IPv4::Send(uint8_t *Data, uint64_t Length, uint8_t Protocol, InternetProtocol4 DestinationIP)
    {
        netdbg("IPv4: Sending %ld bytes to %d.%d.%d.%d", Length, DestinationIP.Address[0], DestinationIP.Address[1], DestinationIP.Address[2], DestinationIP.Address[3]);
        IPv4Packet *Packet = (IPv4Packet *)kmalloc(Length + sizeof(IPv4Header));

        Packet->Header.Version = 4;
        Packet->Header.IHL = sizeof(IPv4Header) / 4;
        Packet->Header.TypeOfService = 0;
        Packet->Header.TotalLength = Length + sizeof(IPv4Header);
        Packet->Header.TotalLength = ((Packet->Header.TotalLength & 0xFF00) >> 8) | ((Packet->Header.TotalLength & 0x00FF) << 8);
        Packet->Header.Identification = 0;
        // Packet->Header.Flags = 0x0;
        // Packet->Header.FragmentOffset = 0x0;
        Packet->Header.FlagsAndFragmentOffset = 0x0;
        Packet->Header.TimeToLive = 64;
        Packet->Header.Protocol = Protocol;
        Packet->Header.DestinationIP = b32(DestinationIP.ToHex());
        Packet->Header.SourceIP = b32(Ethernet->GetInterface()->IP.ToHex());
        Packet->Header.HeaderChecksum = 0x0;
        Packet->Header.HeaderChecksum = CalculateChecksum((uint16_t *)Packet, sizeof(IPv4Header));

        memcpy(Packet->Data, Data, Length);
        InternetProtocol4 DestinationRoute = DestinationIP;
        if ((b32(DestinationIP.ToHex()) & SubnetworkMaskIP.ToHex()) != (Packet->Header.SourceIP & SubnetworkMaskIP.ToHex()))
            DestinationRoute = SubnetworkMaskIP;

        Ethernet->Send(MediaAccessControl().FromHex(ARP->Resolve(DestinationRoute)), this->GetFrameType(), (uint8_t *)Packet, Length + sizeof(IPv4Header));
        kfree(Packet);
    }

    Vector<IPv4Events *> RegisteredEvents;

    bool IPv4::OnEthernetPacketReceived(uint8_t *Data, uint64_t Length)
    {
        IPv4Packet *Packet = (IPv4Packet *)Data;
        netdbg("IPv4: Received %d bytes [Protocol %ld]", Length, Packet->Header.Protocol);
        if (Length < sizeof(IPv4Header))
        {
            warn("IPv4: Packet too short");
            return false;
        }

        bool Reply = false;

        if (Packet->Header.DestinationIP == Ethernet->GetInterface()->IP.ToHex() || Packet->Header.DestinationIP == 0xFFFFFFFF || Ethernet->GetInterface()->IP.ToHex() == 0)
        {
            uint64_t TotalLength = Packet->Header.TotalLength;
            if (TotalLength > Length)
                TotalLength = Length;

            foreach (auto Event in RegisteredEvents)
                if (Packet->Header.Protocol == Event->GetProtocol())
                    if (Event->OnIPv4PacketReceived(InternetProtocol4().FromHex(b32(Packet->Header.SourceIP)), InternetProtocol4().FromHex(b32(Packet->Header.DestinationIP)), (uint8_t *)((uint64_t)Data + 4 * Packet->Header.IHL), TotalLength - 4 * Packet->Header.IHL))
                        Reply = true;
        }

        if (Reply)
        {
            uint32_t SwapIP = Packet->Header.DestinationIP;
            Packet->Header.DestinationIP = Packet->Header.SourceIP;
            Packet->Header.SourceIP = SwapIP;
            Packet->Header.TimeToLive = 0x40;
            Packet->Header.HeaderChecksum = 0x0;
            Packet->Header.HeaderChecksum = CalculateChecksum((uint16_t *)Data, 4 * Packet->Header.TotalLength);
            Ethernet->GetInterface()->Send((uint8_t *)Data, Length);
        }
        return Reply;
    }

    IPv4Events::IPv4Events(IPv4Protocols Protocol)
    {
        this->Protocol = (uint8_t)Protocol;
        RegisteredEvents.push_back(this);
    }

    IPv4Events::~IPv4Events()
    {
        for (uint64_t i = 0; i < RegisteredEvents.size(); i++)
            if (RegisteredEvents[i] == this)
            {
                RegisteredEvents.remove(i);
                break;
            }
    }
}

namespace NetworkIPv6
{

}
