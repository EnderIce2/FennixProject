#include "NetworkController.hpp"

#include <heap.h>

namespace NetworkUDP
{
    struct EventInfo
    {
        Socket *UDPSocket;
        uint16_t Port;
    };
    Vector<EventInfo> RegisteredEvents;

    UDPEvents::UDPEvents() {}

    UDPEvents::~UDPEvents() {}

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    UDP::UDP(NetworkIPv4::IPv4 *ipv4, NetworkInterfaceManager::DeviceInterface *Interface) : NetworkIPv4::IPv4Events(NetworkIPv4::PROTOCOL_UDP)
    {
        netdbg("UDP: Initializing.");
        this->ipv4 = ipv4;
        this->Interface = Interface;
    }

    UDP::~UDP() {}

    uint16_t UsuablePort = 0x200;

    Socket *UDP::Connect(InternetProtocol4 IP, uint16_t Port)
    {
        netdbg("UDP: Connecting to %d.%d.%d.%d:%d", IP.Address[0], IP.Address[1], IP.Address[2], IP.Address[3], Port);
        Socket *socket = new Socket(this);
        socket->RemoteIP = IP;
        socket->RemotePort = Port;
        socket->LocalPort = UsuablePort++; // TODO: track ports
        socket->LocalIP = Interface->IP;
        socket->LocalPort = __builtin_bswap16(socket->LocalPort);
        socket->RemotePort = __builtin_bswap16(socket->RemotePort);
        RegisteredEvents.push_back({.UDPSocket = socket, .Port = socket->LocalPort});
        return socket;
    }

    Socket *UDP::Listen(uint16_t Port)
    {
        warn("Not implemented.");
        return nullptr;
    }

    void UDP::Disconnect(Socket *Socket)
    {
        warn("Not implemented.");
    }

    void UDP::Send(Socket *Socket, uint8_t *Data, uint64_t Length)
    {
        netdbg("UDP: Sending %d bytes to %d.%d.%d.%d:%d", Length, Socket->RemoteIP.Address[0], Socket->RemoteIP.Address[1], Socket->RemoteIP.Address[2], Socket->RemoteIP.Address[3], Socket->RemotePort);
        uint16_t TotalLength = Length + sizeof(UDPHeader);
        UDPPacket *packet = (UDPPacket *)kmalloc(TotalLength);
        packet->Header.SourcePort = Socket->LocalPort;
        packet->Header.DestinationPort = Socket->RemotePort;
        packet->Header.Length = b16(TotalLength);
        memcpy(packet->Data, Data, Length);
        packet->Header.Checksum = 0; // I totaly should do this. Some devices may require it.
        // packet->Header.Checksum = CalculateChecksum((uint16_t *)packet, TotalLength);
        this->ipv4->Send((uint8_t *)packet, TotalLength, 0x11, Socket->RemoteIP);
        kfree(packet);
    }

    void UDP::Bind(Socket *Socket, UDPEvents *EventHandler) { Socket->EventHandler = EventHandler; }

    bool UDP::OnIPv4PacketReceived(InternetProtocol4 SourceIP, InternetProtocol4 DestinationIP, uint8_t *Data, uint64_t Length)
    {
        netdbg("UDP: Received %d bytes from %d.%d.%d.%d", Length, SourceIP.Address[0], SourceIP.Address[1], SourceIP.Address[2], SourceIP.Address[3]);
        if (Length < sizeof(UDPHeader))
            return false;

        UDPHeader *udp = (UDPHeader *)Data;

        netdbg("UDP: SP:%d | DP:%d | L:%d | CHK:%#x", b16(udp->SourcePort), b16(udp->DestinationPort), b16(udp->Length), b16(udp->Checksum));

        Socket *GoodSocket = nullptr;

        foreach (auto var in RegisteredEvents)
        {
            netdbg("UDP->SKT[]: LP:%d | LIP:%d.%d.%d.%d | RP:%d | RIP:%d.%d.%d.%d | LST:%d",
                   b16(var.UDPSocket->LocalPort),
                   var.UDPSocket->LocalIP.Address[0], var.UDPSocket->LocalIP.Address[1], var.UDPSocket->LocalIP.Address[2], var.UDPSocket->LocalIP.Address[3],
                   b16(var.UDPSocket->RemotePort),
                   var.UDPSocket->RemoteIP.Address[0], var.UDPSocket->RemoteIP.Address[1], var.UDPSocket->RemoteIP.Address[2], var.UDPSocket->RemoteIP.Address[3],
                   b16(var.UDPSocket->Listening));
            if (var.UDPSocket->LocalPort == udp->DestinationPort &&
                var.UDPSocket->LocalIP == DestinationIP &&
                var.UDPSocket->Listening == true)
            {
                var.UDPSocket->Listening = false;
                var.UDPSocket->RemotePort = b16(udp->SourcePort);
                var.UDPSocket->RemoteIP = SourceIP;
                netdbg("UDP: E1");
                return true;
            }

            GoodSocket = var.UDPSocket;
        }
        if (GoodSocket)
            GoodSocket->EventHandler->OnUDPPacketReceived(GoodSocket, ((UDPPacket *)Data)->Data, Length);

        netdbg("UDP: E0 (Success)");
        return false;
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    Socket::Socket(UDP *_UDP) { this->SocketUDP = _UDP; }

    Socket::~Socket() {}
}
