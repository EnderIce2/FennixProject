#pragma once

#include <debug.h>
#include <stdint.h>

#define DEBUG_NETWORK 1

#ifdef DEBUG_NETWORK
#define netdbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define netdbg(m, ...)
#endif

enum NetworkOperation
{
    REQUEST = 1,
    REPLY = 2
};

struct MediaAccessControl
{
    uint8_t Address[6];
};

struct InternetProtocol
{
    uint8_t v4Address[4];
    uint8_t v6Address[16];
};

enum CompareIPType
{
    CompareIPv4,
    CompareIPv6
};

static inline bool CompareIP(InternetProtocol IP1, InternetProtocol IP2, CompareIPType Type)
{
    if (Type == CompareIPv4)
        return IP1.v4Address[0] == IP2.v4Address[0] && IP1.v4Address[1] == IP2.v4Address[1] && IP1.v4Address[2] == IP2.v4Address[2] && IP1.v4Address[3] == IP2.v4Address[3];
    else
        for (int i = 0; i < 16; i++)
            if (IP1.v6Address[i] != IP2.v6Address[i])
                return false;
    return true;
}

static inline bool CompareMAC(MediaAccessControl MAC1, MediaAccessControl MAC2)
{
    for (int i = 0; i < 6; i++)
        if (MAC1.Address[i] != MAC2.Address[i])
            return false;
    return true;
}

static inline bool ValidMAC(MediaAccessControl MAC)
{
    if (CompareMAC(MAC, {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}))
        return false;
    if (CompareMAC(MAC, {.Address = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}))
        return false;
    return true;
}

namespace NetworkInterfaceManager
{
    struct DeviceInterface
    {
        char Name[128];
        uint64_t ID;
        MediaAccessControl MAC;
        InternetProtocol IP;

        virtual void Send(void *Data, uint64_t Length)
        {
            warn("Not implemented.");
        }

        virtual void Receive()
        {
            warn("Not implemented.");
        }
    };

#define MAX_INTERFACES 32

    class NetworkInterface
    {
    private:
        int CardIDs = 0;
        DeviceInterface *Devices[MAX_INTERFACES];
        void TraceCards();

    public:
        NetworkInterface();
        ~NetworkInterface();
        void Scan();
        void Send(DeviceInterface Interface, void *Data, int Length);
        void Receive(DeviceInterface Interface, void *Data, int Length);
    };
}

namespace NetworkEthernet
{
    /* https://en.wikipedia.org/wiki/EtherType */

    enum ProtocolType
    {
        ETYPE_IPV4 = 0x0800,
        ETYPE_ARP = 0x0806,
        ETYPE_RARP = 0x8035,
        ETYPE_IPV6 = 0x86DD
    };

    struct EthernetHeader
    {
        MediaAccessControl DestinationMAC;
        MediaAccessControl SourceMAC;
        ProtocolType Type;
    };

    struct EthernetPacket
    {
        EthernetHeader Header;
        uint8_t Data[];
    };

    class Ethernet
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        struct SendInfo
        {
            MediaAccessControl MAC;
            ProtocolType Type;
        };

        Ethernet(NetworkInterfaceManager::DeviceInterface *Interface);
        ~Ethernet();
        void Send(void *Data, int Length, SendInfo Info);
        void Receive(void *Data);
    };
}

namespace NetworkDHCP
{
    struct DHCPHeader
    {
        uint8_t Opcode;
        uint8_t HardwareType;
        uint8_t HardwareAddressLength;
        uint8_t Hops;
        uint32_t TransactionID;
        uint16_t Seconds;
        uint16_t Flags;
        uint32_t ClientIP;
        uint32_t YourIP;
        uint32_t ServerIP;
        uint32_t GatewayIP;
        uint8_t ClientHardwareAddress[16];
        uint8_t ServerHostName[64];
        uint8_t BootFileName[128];
        uint32_t Options[64];
    };

    struct DHCPData
    {
        InternetProtocol IP;
        uint32_t Gateway;
        uint32_t Subnetwork;
        uint32_t DomainNameSystem;
    };

    class DHCP
    {
    public:
        DHCP(NetworkInterfaceManager::DeviceInterface *Interface);
        ~DHCP();
        void Request();
        void Request(InternetProtocol IP);
    };
}

namespace NetworkARP
{
    /* https://en.wikipedia.org/wiki/Address_Resolution_Protocol */
    /* https://www.javatpoint.com/arp-packet-format */

    enum ARPHardwareType
    {
        HTYPE_ETHERNET = 1,
        HTYPE_802_3 = 6,
        HTYPE_ARCNET = 7,
        HTYPE_FRAME_RELAY = 15,
        HTYPE_ATM = 16,
        HTYPE_HDLC = 17,
        HTYPE_FIBRE_CHANNEL = 18,
        HTYPE_ATM_2 = 19,
        HTYPE_SERIAL_LINE = 20
    };

    struct ARPHeader
    {
        ARPHardwareType HardwareType;
        NetworkEthernet::ProtocolType ProtocolType;
        uint8_t HardwareSize;
        uint8_t ProtocolSize;
        uint16_t Operation;
        MediaAccessControl SenderMAC;
        InternetProtocol SenderIP;
        MediaAccessControl TargetMAC;
        InternetProtocol TargetIP;
    };

    struct DiscoveredAddress
    {
        MediaAccessControl MAC;
        InternetProtocol IP;
    };

    class ARP
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        ARP(NetworkInterfaceManager::DeviceInterface *Interface);
        ~ARP();

        DiscoveredAddress *Search(InternetProtocol TargetIP);
        DiscoveredAddress *Update(InternetProtocol TargetIP, MediaAccessControl TargetMAC);

        void Send(InternetProtocol TargetIP, MediaAccessControl TargetMAC);
        void Receive(void *Data);
    };
}

namespace NetworkNTP
{
    struct NTPHeader
    {
        uint8_t LIv;
        uint8_t VN;
        uint8_t Mode;
        uint8_t Stratum;
        uint8_t Poll;
        uint8_t Precision;
        uint32_t RootDelay;
        uint32_t RootDispersion;
        uint32_t ReferenceID;
        uint32_t ReferenceTimestamp;
        uint32_t OriginateTimestamp;
        uint32_t ReceiveTimestamp;
        uint32_t TransmitTimestamp;
    };

    class NTP
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        NTP(NetworkInterfaceManager::DeviceInterface *Interface);
        ~NTP();
    };
}

namespace NetworkIPv4
{
    struct IPv4Header
    {
        uint8_t Version;
        uint8_t IHL;
        uint16_t DSCP_ECN;
        uint16_t TotalLength;
        uint16_t Identification;
        uint16_t Flags_FragmentOffset;
        uint8_t TTL;
        uint8_t Protocol;
        uint16_t Checksum;
        InternetProtocol SourceIP;
        InternetProtocol DestinationIP;
    };

    struct IPv4Packet
    {
        IPv4Header Header;
        uint8_t Data[];
    };
}

namespace NetworkIPv6
{
    struct IPv6Header
    {
        uint32_t Version;
        uint8_t TrafficClass;
        uint16_t FlowLabel;
        uint16_t PayloadLength;
        uint8_t NextHeader;
        uint8_t HopLimit;
        InternetProtocol SourceIP;
        InternetProtocol DestinationIP;
    };

    struct IPv6Packet
    {
        IPv6Header Header;
        uint8_t Data[];
    };
}

namespace NetworkICMPv4
{
    struct ICMPHeader
    {
        uint8_t Type;
        uint8_t Code;
        uint16_t Checksum;
        uint16_t Identifier;
        uint16_t SequenceNumber;
    };

    struct ICMPPacket
    {
        ICMPHeader Header;
        uint8_t Data[];
    };
}

namespace NetworkICMPv6
{
    struct ICMPHeader
    {
        uint8_t Type;
        uint8_t Code;
        uint16_t Checksum;
        uint16_t Identifier;
        uint16_t SequenceNumber;
    };

    struct ICMPPacket
    {
        ICMPHeader Header;
        uint8_t Data[];
    };
}

namespace NetworkTCP
{
}

namespace NetworkUDP
{
}

extern NetworkInterfaceManager::NetworkInterface *nimgr;
