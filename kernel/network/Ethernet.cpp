#include "NetworkController.hpp"

#include <debug.h>
#include <heap.h>

namespace NetworkEthernet
{
    Ethernet::Ethernet(NetworkInterfaceManager::DeviceInterface *Interface) : NetworkInterfaceManager::Events(Interface) { this->Interface = Interface; }
    Ethernet::~Ethernet() {}

    void Ethernet::Send(MediaAccessControl MAC, FrameType Type, uint8_t *Data, uint64_t Length)
    {
        netdbg("ETH: Sending frame type %#x to %02x:%02x:%02x:%02x:%02x:%02x", Type,
               MAC.Address[0], MAC.Address[1], MAC.Address[2],
               MAC.Address[3], MAC.Address[4], MAC.Address[5]);
        uint64_t PacketLength = sizeof(EthernetHeader) + Length;
        EthernetPacket *Packet = (EthernetPacket *)kmalloc(PacketLength);

        Packet->Header.DestinationMAC = b48(MAC.ToHex());
        Packet->Header.SourceMAC = b48(this->Interface->MAC.ToHex());
        Packet->Header.Type = b16(Type);

        memcpy(Packet->Data, Data, Length);
        nimgr->Send(Interface, (uint8_t *)Packet, PacketLength);
        kfree(Packet);
    }

    struct EthernetEventHelperStruct
    {
        EthernetEvents *Ptr;
        uint16_t Type;
    };

    Vector<EthernetEventHelperStruct> RegisteredEvents;

    void Ethernet::Receive(uint8_t *Data, uint64_t Length)
    {
        EthernetPacket *Packet = (EthernetPacket *)Data;
        MediaAccessControl SourceMAC;
        SourceMAC.FromHex(b48(Packet->Header.SourceMAC));
        MediaAccessControl DestinationMAC;
        DestinationMAC.FromHex(b48(Packet->Header.DestinationMAC));

        if (b48(Packet->Header.DestinationMAC) == 0xFFFFFFFFFFFF ||
            b48(Packet->Header.DestinationMAC) == this->Interface->MAC)
        {

            netdbg("ETH: Received data from %02x:%02x:%02x:%02x:%02x:%02x [Type %#x]",
                   SourceMAC.Address[0], SourceMAC.Address[1], SourceMAC.Address[2],
                   SourceMAC.Address[3], SourceMAC.Address[4], SourceMAC.Address[5], b16(Packet->Header.Type));

            bool Reply = false;

            switch (b16(Packet->Header.Type))
            {
            case TYPE_IPV4:
                foreach (auto var in RegisteredEvents)
                    if (var.Type == TYPE_IPV4)
                        Reply = var.Ptr->OnEthernetPacketReceived((uint8_t *)Packet->Data, Length);
                break;
            case TYPE_ARP:
                foreach (auto var in RegisteredEvents)
                    if (var.Type == TYPE_ARP)
                        Reply = var.Ptr->OnEthernetPacketReceived((uint8_t *)Packet->Data, Length);
                break;
            case TYPE_RARP:
                foreach (auto var in RegisteredEvents)
                    if (var.Type == TYPE_RARP)
                        Reply = var.Ptr->OnEthernetPacketReceived((uint8_t *)Packet->Data, Length);
                break;
            case TYPE_IPV6:
                foreach (auto var in RegisteredEvents)
                    if (var.Type == TYPE_IPV6)
                        Reply = var.Ptr->OnEthernetPacketReceived((uint8_t *)Packet->Data, Length);
                break;
            default:
                warn("ETH: Unknown packet type %#lx", Packet->Header.Type);
                break;
            }
            if (Reply)
            {
                Packet->Header.DestinationMAC = Packet->Header.SourceMAC;
                Packet->Header.SourceMAC = b48(this->Interface->MAC.ToHex());
            }
        }
        else
        {
            netdbg("ETH: Type: [%#x] [%02x:%02x:%02x:%02x:%02x:%02x]=>[%02x:%02x:%02x:%02x:%02x:%02x]", b16(Packet->Header.Type),
                   SourceMAC.Address[0], SourceMAC.Address[1], SourceMAC.Address[2],
                   SourceMAC.Address[3], SourceMAC.Address[4], SourceMAC.Address[5],
                   DestinationMAC.Address[0], DestinationMAC.Address[1], DestinationMAC.Address[2],
                   DestinationMAC.Address[3], DestinationMAC.Address[4], DestinationMAC.Address[5]);
        }
    }

    void Ethernet::OnInterfaceReceived(NetworkInterfaceManager::DeviceInterface *Interface, uint8_t *Data, uint64_t Length)
    {
        if (Interface == this->Interface)
            this->Receive(Data, Length);
    }

    EthernetEvents::EthernetEvents(FrameType Type)
    {
        FType = Type;
        RegisteredEvents.push_back({.Ptr = this, .Type = (uint16_t)Type});
    }

    EthernetEvents::~EthernetEvents()
    {
        for (uint64_t i = 0; i < RegisteredEvents.size(); i++)
            if (RegisteredEvents[i].Ptr == this)
            {
                RegisteredEvents.remove(i);
                return;
            }
    }
}
