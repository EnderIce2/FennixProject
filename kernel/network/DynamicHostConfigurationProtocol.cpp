#include "NetworkController.hpp"

#include "../timer.h"

namespace NetworkDHCP
{
    DHCP::DHCP(NetworkUDP::Socket *Socket, NetworkInterfaceManager::DeviceInterface *Interface)
    {
        netdbg("DHCP: Initializing.");
        this->UDPSocket = Socket;
        this->Interface = Interface;
        Socket->LocalPort = b16(68);
    }

    DHCP::~DHCP()
    {
    }

    void DHCP::CreatePacket(DHCPHeader *Packet, uint8_t MessageType, uint32_t RequestIP)
    {
        Packet->Opcode = b8(DHCP_OP_BOOTREQUEST);
        Packet->HardwareType = b8(1);
        Packet->HardwareAddressLength = b8(6);
        Packet->Hops = b8(0);
        Packet->TransactionID = b32(DHCP_TRANSACTION_ID);
        Packet->Flags = b16(0x40);
        uint48_t InterfaceMAC = b48(Interface->MAC.ToHex());
        memcpy(Packet->ClientHardwareAddress, &InterfaceMAC, sizeof(InterfaceMAC));

        uint8_t *Ptr = Packet->Options;
        *((uint32_t *)(Ptr)) = b32(0x63825363); // magic cookie
        Ptr += 4;

        *(Ptr++) = DHCP_OPTION_MESSAGE_TYPE;
        *(Ptr++) = DHCP_MESSAGE_TYPE_DISCOVER;
        *(Ptr++) = MessageType;

        *(Ptr++) = DHCP_OPTION_CLIENT_IDENTIFIER;
        *(Ptr++) = 0x07;
        *(Ptr++) = 0x01;
        memcpy(Ptr, &InterfaceMAC, sizeof(InterfaceMAC));
        Ptr += 6;

        *(Ptr++) = DHCP_OPTION_REQUESTED_IP;
        *(Ptr++) = 0x04;
        *((uint32_t *)(Ptr)) = b32(0x0a00020e);
        memcpy((uint32_t *)(Ptr), &RequestIP, 4);
        Ptr += 4;

        *(Ptr++) = DHCP_OPTION_HOST_NAME;
        char *HostName = (char *)KERNEL_NAME;
        *(Ptr++) = 1 + strlen(HostName);
        memcpy(Ptr, HostName, strlen(HostName));
        Ptr += strlen(HostName);

        *(Ptr++) = DHCP_OPTION_PAD;

        *(Ptr++) = DHCP_OPTION_PARAMETER_REQUEST_LIST;
        *(Ptr++) = DHCP_OPTION_COOKIE_SERVER;
        *(Ptr++) = DHCP_OPTION_SUBNETMASK;
        *(Ptr++) = DHCP_OPTION_ROUTER;
        *(Ptr++) = DHCP_OPTION_DOMAIN_NAME_SERVER;
        *(Ptr++) = DHCP_OPTION_DOMAIN_NAME;
        *(Ptr++) = DHCP_OPTION_NETBIOS_NAME_SERVERS;
        *(Ptr++) = DHCP_OPTION_NETBIOS_NODE_TYPE;
        *(Ptr++) = DHCP_OPTION_NETBIOS_SCOPE;
        *(Ptr++) = DHCP_OPTION_MAX_MESSAGE_SIZE;

        *(Ptr++) = DHCP_OPTION_END;
    }

    void DHCP::Request()
    {
        netdbg("DHCP: Requesting IP address");
        DHCPHeader packet;
        memset(&packet, 0, sizeof(DHCPHeader));

        CreatePacket(&packet, DHCP_MESSAGE_TYPE_DISCOVER, 0x00000000);
        this->UDPSocket->SocketUDP->Send(this->UDPSocket, (uint8_t *)&packet, sizeof(DHCPHeader));

        int RequestTimeout = 10;
        while (!Received)
        {
            if (--RequestTimeout == 0)
            {
                warn("DHCP: Request timeout.");
                break;
            }
            sleep(5);
        }
    }

    void DHCP::Request(InternetProtocol4 IP)
    {
        netdbg("DHCP: Requesting IP address %d.%d.%d.%d", IP.Address[0], IP.Address[1], IP.Address[2], IP.Address[3]);
        DHCPHeader packet;
        memset(&packet, 0, sizeof(DHCPHeader));

        CreatePacket(&packet, DHCP_MESSAGE_TYPE_REQUEST, IP.ToHex());
        UDPSocket->SocketUDP->Send(UDPSocket, (uint8_t *)&packet, sizeof(DHCPHeader));
    }

    void *DHCP::GetOption(DHCPHeader *Packet, uint8_t Type)
    {
        uint8_t *Option = Packet->Options + 4;
        uint8_t Current = *Option;
        while (Current != 0xff)
        {
            uint8_t OptionLength = *(Option + 1);
            if (Current == Type)
                return Option + 2;
            Option += (2 + OptionLength);
            Current = *Option;
        }
        warn("DHCP: Option %#x not found", Type);
        return nullptr;
    }

    void DHCP::OnUDPPacketReceived(NetworkUDP::Socket *Socket, uint8_t *Data, uint64_t Length)
    {
        DHCPHeader *Packet = (DHCPHeader *)Data;

        uint8_t *MessageType = (uint8_t *)GetOption(Packet, DHCP_OPTION_MESSAGE_TYPE);

        switch (*MessageType)
        {
        case DHCP_OPTION_TIME_OFFSET:
            this->Request(InternetProtocol4().FromHex(Packet->YourIP));
            break;
        case DHCP_OPTION_NAME_SERVER:
            this->IP.FromHex(Packet->YourIP);
            this->Gateway.FromHex(*(uint32_t *)GetOption(Packet, DHCP_OPTION_ROUTER));
            this->DomainNameSystem.FromHex(*(uint32_t *)GetOption(Packet, DHCP_OPTION_DOMAIN_NAME_SERVER));
            this->SubnetworkMask.FromHex((*(uint32_t *)GetOption(Packet, DHCP_OPTION_SUBNETMASK)));
            this->Received = true;
            break;
        default:
            netdbg("DHCP: Received unknown message type %#x", *MessageType);
            break;
        }
    }
}
