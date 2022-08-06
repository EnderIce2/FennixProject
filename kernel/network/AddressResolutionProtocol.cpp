#include "NetworkController.hpp"

#include <debug.h>

Vector<NetworkARP::DiscoveredAddress *> DiscoveredAddresses;

namespace NetworkARP
{
    enum DAType
    {
        DA_ADD = 1,
        DA_DEL = 2,
        DA_SEARCH = 3,
        DA_UPDATE = 4
    };

    DiscoveredAddress *ManageDA(DAType Type, InternetProtocol IP, MediaAccessControl MAC)
    {
        // TODO: Compare IPv6 too.
        DiscoveredAddress *tmp = new DiscoveredAddress;
        switch (Type)
        {
        case DA_ADD:
            tmp->IP = IP;
            tmp->MAC = MAC;
            DiscoveredAddresses.push_back(tmp);
            return tmp;
        case DA_DEL:
            for (uint64_t i = 0; i < DiscoveredAddresses.size(); i++)
                if (CompareIP(DiscoveredAddresses[i]->IP, IP, CompareIPv4))
                {
                    DiscoveredAddress *tmp = DiscoveredAddresses[i];
                    DiscoveredAddresses.remove(i);
                    return tmp;
                }
            delete tmp;
            return nullptr;
        case DA_SEARCH:
            delete tmp;
            for (uint64_t i = 0; i < DiscoveredAddresses.size(); i++)
                if (CompareIP(DiscoveredAddresses[i]->IP, IP, CompareIPv4))
                {
                    return DiscoveredAddresses[i];
                }
            return nullptr;
        case DA_UPDATE:
            delete tmp;
            for (uint64_t i = 0; i < DiscoveredAddresses.size(); i++)
                if (CompareIP(DiscoveredAddresses[i]->IP, IP, CompareIPv4))
                {
                    DiscoveredAddresses[i]->MAC = MAC;
                    return DiscoveredAddresses[i];
                }
            return nullptr;
        }
        delete tmp;
        return nullptr;
    }

    ARP::ARP(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }

    ARP::~ARP() {}

    DiscoveredAddress *ARP::Search(InternetProtocol TargetIP) { return ManageDA(DA_SEARCH, TargetIP, MediaAccessControl()); }

    DiscoveredAddress *ARP::Update(InternetProtocol TargetIP, MediaAccessControl TargetMAC) { return ManageDA(DA_UPDATE, TargetIP, TargetMAC); }

    void ARP::Send(InternetProtocol TargetIP, MediaAccessControl TargetMAC)
    {
        // TODO: delete object after sending
        ARPHeader *Header = new ARPHeader;
        Header->HardwareType = ARPHardwareType::HTYPE_ETHERNET;
        Header->ProtocolType = NetworkEthernet::ProtocolType::ETYPE_IPV4;
        Header->HardwareSize = 6;
        Header->ProtocolSize = 4;
        Header->Operation = NetworkOperation::REQUEST;
        Header->SenderMAC = Interface->MAC;
        Header->SenderIP = Interface->IP;
        Header->TargetMAC = TargetMAC;
        Header->TargetIP = TargetIP;
        NetworkEthernet::Ethernet(Interface).Send(Header, sizeof(ARPHeader), {.MAC = TargetMAC, .Type = NetworkEthernet::ProtocolType::ETYPE_ARP});
    }

    void ARP::Receive(void *Data)
    {
        ARPHeader *Header = (ARPHeader *)Data;
        if (Header->HardwareType != ARPHardwareType::HTYPE_ETHERNET || Header->ProtocolType != NetworkEthernet::ProtocolType::ETYPE_IPV4)
        {
            warn("ARP: Invalid hardware/protocol type (%d/%d)", Header->HardwareType, Header->ProtocolType);
            return;
        }

        if (ManageDA(DA_SEARCH, Header->SenderIP, Header->SenderMAC) == nullptr)
        {
            netdbg("ARP: Discovered new address %d.%d.%d.%d", Header->SenderIP.v4Address[0], Header->SenderIP.v4Address[1], Header->SenderIP.v4Address[2], Header->SenderIP.v4Address[3]);
            ManageDA(DA_ADD, Header->SenderIP, Header->SenderMAC);
        }
        else
        {
            netdbg("ARP: Updated address %d.%d.%d.%d", Header->SenderIP.v4Address[0], Header->SenderIP.v4Address[1], Header->SenderIP.v4Address[2], Header->SenderIP.v4Address[3]);
            ManageDA(DA_UPDATE, Header->SenderIP, Header->SenderMAC);
        }

        switch (Header->Operation)
        {
        case NetworkOperation::REQUEST:
            netdbg("ARP: Received request for %d.%d.%d.%d", Header->TargetIP.v4Address[0], Header->TargetIP.v4Address[1], Header->TargetIP.v4Address[2], Header->TargetIP.v4Address[3]);
            Header->TargetMAC = Header->SenderMAC;
            Header->TargetIP = Header->SenderIP;
            Header->SenderMAC = Interface->MAC;
            Header->SenderIP = Interface->IP;
            Header->Operation = NetworkOperation::REPLY;
            NetworkEthernet::Ethernet(Interface).Send(Header, sizeof(ARPHeader), {.MAC = Header->TargetMAC, .Type = NetworkEthernet::ProtocolType::ETYPE_ARP});
            break;
        case NetworkOperation::REPLY:
            fixme("ARP: Received reply for %d.%d.%d.%d", Header->TargetIP.v4Address[0], Header->TargetIP.v4Address[1], Header->TargetIP.v4Address[2], Header->TargetIP.v4Address[3]);
            break;
        }
    }
}
