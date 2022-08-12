#include "NetworkController.hpp"

#include "../timer.h"

#include <debug.h>

namespace NetworkARP
{
    DiscoveredAddress *ARP::ManageDA(DAType Type, InternetProtocol4 IP, MediaAccessControl MAC)
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
                if (DiscoveredAddresses[i]->IP == IP)
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
                if (DiscoveredAddresses[i]->IP == IP)
                {
                    return DiscoveredAddresses[i];
                }
            return nullptr;
        case DA_UPDATE:
            delete tmp;
            for (uint64_t i = 0; i < DiscoveredAddresses.size(); i++)
                if (DiscoveredAddresses[i]->IP == IP)
                {
                    DiscoveredAddresses[i]->MAC = MAC;
                    return DiscoveredAddresses[i];
                }
            return nullptr;
        }
        delete tmp;
        return nullptr;
    }

    ARP::ARP(NetworkEthernet::Ethernet *Ethernet) : NetworkEthernet::EthernetEvents(NetworkEthernet::TYPE_ARP)
    {
        netdbg("ARP: Initializing.");
        this->Ethernet = Ethernet;
    }

    ARP::~ARP()
    {
    }

    MediaAccessControl InvalidMAC = {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    InternetProtocol4 InvalidIP = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
    DiscoveredAddress InvalidRet = {.MAC = InvalidMAC, .IP = InvalidIP};

    DiscoveredAddress *ARP::Search(InternetProtocol4 TargetIP)
    {
        DiscoveredAddress *ret = ManageDA(DA_SEARCH, TargetIP, MediaAccessControl());
        if (ret)
            return ret;
        netdbg("ARP: [DA] No address found for %d.%d.%d.%d", TargetIP.Address[0], TargetIP.Address[1], TargetIP.Address[2], TargetIP.Address[3]);
        return &InvalidRet;
    }

    DiscoveredAddress *ARP::Update(InternetProtocol4 TargetIP, MediaAccessControl TargetMAC)
    {
        DiscoveredAddress *ret = ManageDA(DA_UPDATE, TargetIP, TargetMAC);
        if (ret)
            return ret;
        warn("ARP: [DA] No address found for %d.%d.%d.%d", TargetIP.Address[0], TargetIP.Address[1], TargetIP.Address[2], TargetIP.Address[3]);
        return &InvalidRet;
    }

    uint48_t ARP::Resolve(InternetProtocol4 IP)
    {
        netdbg("ARP: Resolving %d.%d.%d.%d", IP.Address[3], IP.Address[2], IP.Address[1], IP.Address[0]);
        if (IP == 0xFFFFFFFF)
            return 0xFFFFFFFFFFFF;

        uint48_t ret = this->Search(IP)->MAC.ToHex();
        netdbg("ARP: Resolved %d.%d.%d.%d to %x", IP.Address[3], IP.Address[2], IP.Address[1], IP.Address[0], ret);

        if (ret == 0xFFFFFFFFFFFF)
        {
            ARPHeader *Header = new ARPHeader;
            Header->HardwareType = ARPHardwareType::HTYPE_ETHERNET;
            Header->ProtocolType = NetworkEthernet::FrameType::TYPE_IPV4;
            Header->HardwareSize = 6;
            Header->ProtocolSize = 4;
            Header->Operation = ARPOperation::REQUEST;
            Header->SenderMAC = Ethernet->GetInterface()->MAC.ToHex();
            Header->SenderIP = Ethernet->GetInterface()->IP.ToHex();
            Header->TargetMAC = 0xFFFFFFFFFFFF;
            Header->TargetIP = IP.ToHex();
            Ethernet->Send({.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, NetworkEthernet::FrameType::TYPE_ARP, (uint8_t *)Header, sizeof(ARPHeader));
            delete Header;
            netdbg("ARP: Sent request");
        }

        int RequestTimeout = 10;
        while (ret == 0xFFFFFFFFFFFF)
        {
            ret = this->Search(IP)->MAC.ToHex();
            if (--RequestTimeout == 0)
            {
                warn("ARP: Request timeout.");
                return 0;
            }
            sleep(5);
        }

        return ret;
    }

    void ARP::Broadcast(InternetProtocol4 IP)
    {
        netdbg("ARP: Sending broadcast");
        uint64_t ResolvedMAC = this->Resolve(IP);
        ARPHeader *Header = new ARPHeader;
        Header->HardwareType = b16(ARPHardwareType::HTYPE_ETHERNET);
        Header->ProtocolType = b16(NetworkEthernet::FrameType::TYPE_IPV4);
        Header->HardwareSize = b8(0x6);
        Header->ProtocolSize = b8(0x4);
        Header->Operation = b16(ARPOperation::REQUEST);
        Header->SenderMAC = b48(Ethernet->GetInterface()->MAC.ToHex());
        Header->SenderIP = b32(Ethernet->GetInterface()->IP.ToHex());
        Header->TargetMAC = ResolvedMAC;
        Header->TargetIP = IP.ToHex();
        Ethernet->Send(MediaAccessControl().FromHex(ResolvedMAC), NetworkEthernet::FrameType::TYPE_ARP, (uint8_t *)Header, sizeof(ARPHeader));
        delete Header;
    }

    bool ARP::OnEthernetPacketReceived(uint8_t *Data, uint64_t Length)
    {
        netdbg("ARP: Received packet");
        ARPHeader *Header = (ARPHeader *)Data;

        InternetProtocol4 SenderIPv4;
        SenderIPv4.FromHex(b32(Header->SenderIP));

        if (b16(Header->HardwareType) != ARPHardwareType::HTYPE_ETHERNET || b16(Header->ProtocolType) != NetworkEthernet::FrameType::TYPE_IPV4)
        {
            warn("ARP: [DA] Invalid hardware/protocol type (%d/%d)", Header->HardwareType, Header->ProtocolType);
            return false;
        }

        if (ManageDA(DA_SEARCH, InternetProtocol4().FromHex(Header->SenderIP), MediaAccessControl().FromHex(Header->SenderMAC)) == nullptr)
        {
            netdbg("ARP: [DA] Discovered new address %d.%d.%d.%d", SenderIPv4.Address[3], SenderIPv4.Address[2], SenderIPv4.Address[1], SenderIPv4.Address[0]);
            ManageDA(DA_ADD, InternetProtocol4().FromHex(Header->SenderIP), MediaAccessControl().FromHex(Header->SenderMAC));
        }
        else
        {
            netdbg("ARP: [DA] Updated address %d.%d.%d.%d", SenderIPv4.Address[3], SenderIPv4.Address[2], SenderIPv4.Address[1], SenderIPv4.Address[0]);
            ManageDA(DA_UPDATE, InternetProtocol4().FromHex(Header->SenderIP), MediaAccessControl().FromHex(Header->SenderMAC));
        }

        switch (b16(Header->Operation))
        {
        case ARPOperation::REQUEST:
            netdbg("ARP: Received request from %d.%d.%d.%d", SenderIPv4.Address[3], SenderIPv4.Address[2], SenderIPv4.Address[1], SenderIPv4.Address[0]);
            Header->TargetMAC = Header->SenderMAC;
            Header->TargetIP = Header->SenderIP;
            Header->SenderMAC = b48(Ethernet->GetInterface()->MAC.ToHex());
            Header->SenderIP = b32(Ethernet->GetInterface()->IP.ToHex());
            Header->Operation = b16(ARPOperation::REPLY);
            Ethernet->Send(MediaAccessControl().FromHex(Header->TargetMAC), NetworkEthernet::FrameType::TYPE_ARP, (uint8_t *)Header, sizeof(ARPHeader));
            netdbg("ARP: Sent request for %d.%d.%d.%d", SenderIPv4.Address[0], SenderIPv4.Address[1], SenderIPv4.Address[2], SenderIPv4.Address[3]);
            break;
        case ARPOperation::REPLY:
            fixme("ARP: Received reply from %d.%d.%d.%d", SenderIPv4.Address[0], SenderIPv4.Address[1], SenderIPv4.Address[2], SenderIPv4.Address[3]);
            break;
        default:
            warn("ARP: Invalid operation (%d)", Header->Operation);
            break;
        }
        return false;
    }
}
