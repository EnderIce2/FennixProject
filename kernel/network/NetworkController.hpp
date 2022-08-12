#pragma once
/*
                    Documentations & Sources
- https://web.archive.org/web/20051210132103/http://users.pcnet.ro/dmoroian/beej/Beej.html
- https://web.archive.org/web/20060229214053/http://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
- https://en.wikipedia.org/wiki/EtherType
- https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/performance_tuning_guide/s-network-packet-reception
- https://linux-kernel-labs.github.io/refs/heads/master/labs/networking.html
- https://github.com/smoltcp-rs/smoltcp
- https://www.ciscopress.com/articles/article.asp?p=3089352&seqNum=5
- https://www.cs.unh.edu/cnrg/people/gherrin/linux-net.html
- https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
- https://github.com/TheUltimateFoxOS/horizon
- https://en.wikipedia.org/wiki/Address_Resolution_Protocol
- https://en.cppreference.com/w/cpp/language/operators
- https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol
- https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139D_DataSheet.pdf
- https://www.javatpoint.com/arp-packet-format
- https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139_ProgrammersGuide.pdf
- http://realtek.info/pdf/rtl8139cp.pdf
- https://en.wikipedia.org/wiki/IPv4
- https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml
*/

#include <debug.h>
#include <string.h>
#include <vector.hpp>

// #define DEBUG_NETWORK 1

#ifdef DEBUG_NETWORK
#define netdbg(m, ...) debug(m, ##__VA_ARGS__)
void DbgNetwork();
void DbgDumpData(const char *Description, void *Address, unsigned long Length);
#else
#define netdbg(m, ...)
static inline void DbgNetwork() { return; }
static inline void DbgDumpData(const char *Description, void *Address, unsigned long Length) { return; }
#endif

// TODO: How i whould do this?
typedef __UINT64_TYPE__ uint48_t;

#define b4(x) ((x & 0x0F) << 4 | (x & 0xF0) >> 4)
#define b8(x) ((x)&0xFF)
#define b16(x) __builtin_bswap16(x)
#define b32(x) __builtin_bswap32(x)
#define b48(x) (((((x)&0x0000000000ff) << 40) | (((x)&0x00000000ff00) << 24) | (((x)&0x000000ff0000) << 8) | (((x)&0x0000ff000000) >> 8) | (((x)&0x00ff00000000) >> 24) | (((x)&0xff0000000000) >> 40)))
#define b64(x) __builtin_bswap64(x)

struct MediaAccessControl
{
    uint8_t Address[6];

    inline bool operator==(const MediaAccessControl &lhs) const
    {
        return lhs.Address[0] == this->Address[0] &&
               lhs.Address[1] == this->Address[1] &&
               lhs.Address[2] == this->Address[2] &&
               lhs.Address[3] == this->Address[3] &&
               lhs.Address[4] == this->Address[4] &&
               lhs.Address[5] == this->Address[5];
    }

    inline bool operator==(const uint48_t &lhs) const
    {
        MediaAccessControl MAC;
        MAC.Address[0] = (uint8_t)((lhs >> 40) & 0xFF);
        MAC.Address[1] = (uint8_t)((lhs >> 32) & 0xFF);
        MAC.Address[2] = (uint8_t)((lhs >> 24) & 0xFF);
        MAC.Address[3] = (uint8_t)((lhs >> 16) & 0xFF);
        MAC.Address[4] = (uint8_t)((lhs >> 8) & 0xFF);
        MAC.Address[5] = (uint8_t)(lhs & 0xFF);
        return MAC.Address[0] == this->Address[0] &&
               MAC.Address[1] == this->Address[1] &&
               MAC.Address[2] == this->Address[2] &&
               MAC.Address[3] == this->Address[3] &&
               MAC.Address[4] == this->Address[4] &&
               MAC.Address[5] == this->Address[5];
    }

    inline bool operator!=(const MediaAccessControl &lhs) const { return !(*this == lhs); }
    inline bool operator!=(const uint48_t &lhs) const { return !(*this == lhs); }

    inline uint48_t ToHex()
    {
        return ((uint48_t)this->Address[0] << 40) |
               ((uint48_t)this->Address[1] << 32) |
               ((uint48_t)this->Address[2] << 24) |
               ((uint48_t)this->Address[3] << 16) |
               ((uint48_t)this->Address[4] << 8) |
               ((uint48_t)this->Address[5]);
    }

    inline MediaAccessControl FromHex(uint48_t Hex)
    {
        this->Address[0] = (uint8_t)((Hex >> 40) & 0xFF);
        this->Address[1] = (uint8_t)((Hex >> 32) & 0xFF);
        this->Address[2] = (uint8_t)((Hex >> 24) & 0xFF);
        this->Address[3] = (uint8_t)((Hex >> 16) & 0xFF);
        this->Address[4] = (uint8_t)((Hex >> 8) & 0xFF);
        this->Address[5] = (uint8_t)(Hex & 0xFF);
        return *this;
    }

    inline bool Valid()
    {
        // TODO: More complex MAC validation
        return (this->Address[0] != 0 ||
                this->Address[1] != 0 ||
                this->Address[2] != 0 ||
                this->Address[3] != 0 ||
                this->Address[4] != 0 ||
                this->Address[5] != 0) &&
               (this->Address[0] != 0xFF ||
                this->Address[1] != 0xFF ||
                this->Address[2] != 0xFF ||
                this->Address[3] != 0xFF ||
                this->Address[4] != 0xFF ||
                this->Address[5] != 0xFF);
    }
};

struct InternetProtocol4
{
    uint8_t Address[4];

    inline bool operator==(const InternetProtocol4 &lhs) const
    {
        return lhs.Address[0] == this->Address[0] &&
               lhs.Address[1] == this->Address[1] &&
               lhs.Address[2] == this->Address[2] &&
               lhs.Address[3] == this->Address[3];
    }

    inline bool operator==(const uint32_t &lhs) const
    {
        InternetProtocol4 IP;
        IP.Address[0] = (uint8_t)((lhs >> 24) & 0xFF);
        IP.Address[1] = (uint8_t)((lhs >> 16) & 0xFF);
        IP.Address[2] = (uint8_t)((lhs >> 8) & 0xFF);
        IP.Address[3] = (uint8_t)(lhs & 0xFF);

        return IP.Address[0] == this->Address[0] &&
               IP.Address[1] == this->Address[1] &&
               IP.Address[2] == this->Address[2] &&
               IP.Address[3] == this->Address[3];
    }

    inline bool operator!=(const InternetProtocol4 &lhs) const { return !(*this == lhs); }
    inline bool operator!=(const uint32_t &lhs) const { return !(*this == lhs); }

    inline uint32_t ToHex()
    {
        return ((uint64_t)this->Address[0] << 24) |
               ((uint64_t)this->Address[1] << 16) |
               ((uint64_t)this->Address[2] << 8) |
               ((uint64_t)this->Address[3]);
    }

    inline InternetProtocol4 FromHex(uint32_t Hex)
    {
        this->Address[0] = (uint8_t)((Hex >> 24) & 0xFF);
        this->Address[1] = (uint8_t)((Hex >> 16) & 0xFF);
        this->Address[2] = (uint8_t)((Hex >> 8) & 0xFF);
        this->Address[3] = (uint8_t)(Hex & 0xFF);
        return *this;
    }
};

static inline uint16_t CalculateChecksum(uint16_t *Data, uint64_t Length)
{
    uint16_t *Data16 = (uint16_t *)Data;
    uint64_t Checksum = 0;
    for (uint64_t i = 0; i < Length / 2; i++)
        Checksum += ((Data16[i] & 0xFF00) >> 8) | ((Data16[i] & 0x00FF) << 8);
    if (Length % 2)
        Checksum += ((uint16_t)((char *)Data16)[Length - 1]) << 8;
    while (Checksum & 0xFFFF0000)
        Checksum = (Checksum & 0xFFFF) + (Checksum >> 16);
    return (uint16_t)(((~Checksum & 0xFF00) >> 8) | ((~Checksum & 0x00FF) << 8));
}

namespace NetworkInterfaceManager
{
    struct DeviceInterface
    {
        /** @brief Device interface name */
        char Name[128];
        /** @brief Device interface index */
        uint64_t ID;
        /** @brief Device interface MAC address (Big-endian) */
        MediaAccessControl MAC;
        /** @brief Device interface IP address (Big-endian) */
        InternetProtocol4 IP;

        virtual void Send(void *Data, uint64_t Length) { warn("Driver doesn't have Send( %p %p ) implemented!", Data, Length); }
        virtual void Receive() { warn("Driver doesn't have Receive() implemented!"); }
    };

#define MAX_INTERFACES 32

    class Events
    {
    protected:
        Events(DeviceInterface *Interface);
        ~Events();

    public:
        virtual void OnInterfaceAdded(DeviceInterface *Interface) { netdbg("Event for %s not handled.", Interface->Name); }
        virtual void OnInterfaceRemoved(DeviceInterface *Interface) { netdbg("Event for %s not handled.", Interface->Name); }
        virtual void OnInterfaceChanged(DeviceInterface *Interface) { netdbg("Event for %s not handled.", Interface->Name); }
        virtual void OnInterfaceReceived(DeviceInterface *Interface, uint8_t *Data, uint64_t Length) { netdbg("Event for %s not handled.", Interface->Name); }
        virtual void OnInterfaceSent(DeviceInterface *Interface, uint8_t *Data, uint64_t Length) { netdbg("Event for %s not handled.", Interface->Name); }
    };

    class NetworkInterface
    {
    private:
        int CardIDs = 0;
        DeviceInterface *Devices[MAX_INTERFACES];
        void TraceCards();

    public:
        NetworkInterface();
        ~NetworkInterface();

        void StartNetworkStack();
        void StopNetworkStack();

        void Send(DeviceInterface *Interface, uint8_t *Data, uint64_t Length);
        void Receive(DeviceInterface *Interface, uint8_t *Data, uint64_t Length);
    };
}

namespace NetworkEthernet
{
    enum FrameType
    {
        TYPE_IPV4 = 0x0800,
        TYPE_ARP = 0x0806,
        TYPE_RARP = 0x8035,
        TYPE_IPV6 = 0x86DD
    };

    struct EthernetHeader
    {
        uint48_t DestinationMAC : 48;
        uint48_t SourceMAC : 48;
        uint16_t Type;
    } __attribute__((packed));

    struct EthernetPacket
    {
        EthernetHeader Header;
        uint8_t Data[];
    };

    class EthernetEvents
    {
    private:
        FrameType FType;

    protected:
        EthernetEvents(FrameType Type);
        ~EthernetEvents();

    public:
        FrameType GetFrameType() { return FType; }
        virtual void OnEthernetPacketSent(EthernetPacket *Packet) { netdbg("Event not handled. [%p]", Packet); }
        virtual bool OnEthernetPacketReceived(uint8_t *Data, uint64_t Length)
        {
            netdbg("Event not handled. [%p, %d]", Data, Length);
            return false;
        }
    };

    class Ethernet : public NetworkInterfaceManager::Events
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;
        void Receive(uint8_t *Data, uint64_t Length);
        void OnInterfaceReceived(NetworkInterfaceManager::DeviceInterface *Interface, uint8_t *Data, uint64_t Length);

    public:
        /** @brief Get driver interface
         * @return Driver interface
         */
        NetworkInterfaceManager::DeviceInterface *GetInterface() { return this->Interface; }

        Ethernet(NetworkInterfaceManager::DeviceInterface *Interface);
        ~Ethernet();

        /**
         * @brief Send an Ethernet packet.
         *
         * @param MAC The MAC address of the destination. (Big-endian)
         * @param Type The type of the packet.
         * @param Data The data to send.
         * @param Length The length of the data.
         */
        void Send(MediaAccessControl MAC, FrameType Type, uint8_t *Data, uint64_t Length);
    };
}

namespace NetworkARP
{
    enum ARPOperation
    {
        REQUEST = 0x1,
        REPLY = 0x2
    };

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
        uint16_t HardwareType;
        uint16_t ProtocolType;
        uint8_t HardwareSize;
        uint8_t ProtocolSize;
        uint16_t Operation;
        uint48_t SenderMAC : 48;
        uint32_t SenderIP;
        uint48_t TargetMAC : 48;
        uint32_t TargetIP;
    } __attribute__((packed));

    struct DiscoveredAddress
    {
        MediaAccessControl MAC;
        InternetProtocol4 IP;
    };

    class ARP : public NetworkEthernet::EthernetEvents
    {
    private:
        NetworkEthernet::Ethernet *Ethernet;

        enum DAType
        {
            DA_ADD = 1,
            DA_DEL = 2,
            DA_SEARCH = 3,
            DA_UPDATE = 4
        };

        Vector<NetworkARP::DiscoveredAddress *> DiscoveredAddresses;
        DiscoveredAddress *ManageDA(DAType Type, InternetProtocol4 IP, MediaAccessControl MAC);
        DiscoveredAddress *Search(InternetProtocol4 TargetIP);
        DiscoveredAddress *Update(InternetProtocol4 TargetIP, MediaAccessControl TargetMAC);
        bool OnEthernetPacketReceived(uint8_t *Data, uint64_t Length);

    public:
        ARP(NetworkEthernet::Ethernet *Ethernet);
        ~ARP();

        /**
         * @brief Resolve an IP address to a MAC address.
         *
         * @param IP The IP address to resolve. (Little-endian)
         * @return uint48_t The MAC address of the IP address.
         */
        uint48_t Resolve(InternetProtocol4 IP);

        /**
         * @brief Broadcast an ARP packet.
         *
         * @param IP The IP address to broadcast.
         */
        void Broadcast(InternetProtocol4 IP);
    };
}

namespace NetworkIPv4
{
    struct IPv4Header
    {
        uint8_t IHL : 4;
        uint8_t Version : 4;
        uint8_t TypeOfService;
        uint16_t TotalLength;
        uint16_t Identification;
        uint16_t FlagsAndFragmentOffset;
        uint8_t TimeToLive;
        uint8_t Protocol;
        uint16_t HeaderChecksum;
        uint32_t SourceIP;
        uint32_t DestinationIP;

        /* On wikipedia page we have this: https://en.wikipedia.org/wiki/File:IPv4_Packet-en.svg
           but only the code above works... */
        // uint8_t Version : 4;
        // uint8_t IHL : 4;
        // uint16_t TypeOfService : 8;
        // uint16_t TotalLength : 12;
        // uint16_t Identification : 16;
        // uint16_t Flags : 3;
        // uint16_t FragmentOffset : 13;
        // uint8_t TimeToLive : 8;
        // uint8_t Protocol : 8;
        // uint16_t HeaderChecksum;
        // uint32_t SourceIP;
        // uint32_t DestinationIP;
    };

    struct IPv4Packet
    {
        IPv4Header Header;
        uint8_t Data[];
    };

    enum IPv4Protocols
    {
        PROTOCOL_ICMP = 1,
        PROTOCOL_IGMP = 2,
        PROTOCOL_TCP = 6,
        PROTOCOL_UDP = 17,
        PROTOCOL_IPV6 = 41,
        PROTOCOL_ROUTING = 43,
        PROTOCOL_FRAGMENT = 44,
        PROTOCOL_ESP = 50,
        PROTOCOL_AH = 51,
        PROTOCOL_ICMPV6 = 58,
        PROTOCOL_NONE = 59,
        PROTOCOL_DSTOPTS = 60,
        PROTOCOL_ND = 77,
        PROTOCOL_ICLFXBM = 78,
        PROTOCOL_PIM = 103,
        PROTOCOL_COMP = 108,
        PROTOCOL_SCTP = 132,
        PROTOCOL_UDPLITE = 136,
        PROTOCOL_RAW = 255
    };

    class IPv4 : public NetworkEthernet::EthernetEvents
    {
    private:
        NetworkARP::ARP *ARP;
        NetworkEthernet::Ethernet *Ethernet;

        virtual bool OnEthernetPacketReceived(uint8_t *Data, uint64_t Length);

    public:
        InternetProtocol4 GatewayIP = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
        InternetProtocol4 SubnetworkMaskIP = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
        IPv4(NetworkARP::ARP *ARP, NetworkEthernet::Ethernet *Ethernet);
        ~IPv4();

        /**
         * @brief Send an IPv4 packet.
         *
         * @param Data The data to send.
         * @param Length The length of the data.
         * @param Protocol The protocol of the packet.
         * @param DestinationIP The IP address of the destination. (Big-endian)
         */
        void Send(uint8_t *Data, uint64_t Length, uint8_t Protocol, InternetProtocol4 DestinationIP);
    };

    class IPv4Events
    {
    private:
        uint8_t Protocol;

    protected:
        IPv4Events(IPv4Protocols Protocol);
        ~IPv4Events();

    public:
        uint8_t GetProtocol() { return Protocol; }

        virtual bool OnIPv4PacketReceived(InternetProtocol4 SourceIP, InternetProtocol4 DestinationIP, uint8_t *Data, uint64_t Length)
        {
            warn("Not implemented.");
            return false;
        }
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
        uint32_t SourceIP;
        uint32_t DestinationIP;
    };

    struct IPv6Packet
    {
        IPv6Header Header;
        uint8_t Data[];
    };
}

namespace NetworkICMPv4
{
    enum ICMPv4Type
    {
        TYPE_ECHO_REPLY = 0,
        TYPE_DESTINATION_UNREACHABLE = 3,
        TYPE_SOURCE_QUENCH = 4,
        TYPE_REDIRECT = 5,
        TYPE_ECHO = 8,
        TYPE_ROUTER_ADVERTISEMENT = 9,
        TYPE_ROUTER_SELECTION = 10,
        TYPE_TIME_EXCEEDED = 11,
        TYPE_PARAMETER_PROBLEM = 12,
        TYPE_TIMESTAMP = 13,
        TYPE_TIMESTAMP_REPLY = 14
    };

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

    class ICMPv4
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        NetworkInterfaceManager::DeviceInterface *GetInterface() { return this->Interface; }

        ICMPv4(NetworkInterfaceManager::DeviceInterface *Interface);
        ~ICMPv4();
        void Send(/* ???? */);
        void Receive(ICMPPacket *Packet);
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

    class ICMPv6
    {
    private:
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        NetworkInterfaceManager::DeviceInterface *GetInterface() { return this->Interface; }

        ICMPv6(NetworkInterfaceManager::DeviceInterface *Interface);
        ~ICMPv6();
        void Send(uint8_t *Data, uint64_t Length);
        void Receive(uint8_t *Data);
    };
}

namespace NetworkTCP
{
}

namespace NetworkUDP
{
    struct UDPHeader
    {
        uint16_t SourcePort;
        uint16_t DestinationPort;
        uint16_t Length;
        uint16_t Checksum;
    } __attribute__((packed));

    struct UDPPacket
    {
        UDPHeader Header;
        uint8_t Data[];
    };

    class Socket;

    class UDPEvents
    {
    protected:
        UDPEvents();
        ~UDPEvents();

    public:
        virtual void OnUDPPacketReceived(Socket *Socket, uint8_t *Data, uint64_t Length)
        {
            warn("Not implemented.");
        }
    };

    class UDP : public NetworkIPv4::IPv4Events
    {
    private:
        NetworkIPv4::IPv4 *ipv4;
        NetworkInterfaceManager::DeviceInterface *Interface;

    public:
        NetworkInterfaceManager::DeviceInterface *GetInterface() { return this->Interface; }

        UDP(NetworkIPv4::IPv4 *ipv4, NetworkInterfaceManager::DeviceInterface *Interface);
        ~UDP();

        virtual Socket *Connect(InternetProtocol4 IP, uint16_t Port);
        virtual Socket *Listen(uint16_t Port);
        virtual void Disconnect(Socket *Socket);
        virtual void Send(Socket *Socket, uint8_t *Data, uint64_t Length);
        virtual void Bind(Socket *Socket, UDPEvents *EventHandler);

        virtual bool OnIPv4PacketReceived(InternetProtocol4 SourceIP, InternetProtocol4 DestinationIP, uint8_t *Data, uint64_t Length);
    };

    class Socket
    {
    public:
        InternetProtocol4 LocalIP = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
        uint16_t LocalPort = 0;
        InternetProtocol4 RemoteIP = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
        uint16_t RemotePort = 0;
        bool Listening = false;
        UDPEvents *EventHandler = nullptr;
        UDP *SocketUDP = nullptr;

        Socket(UDP *_UDP);
        ~Socket();
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
        uint8_t Options[64];
    } __attribute__((packed));

    enum DHCPOperation
    {
        DHCP_OP_BOOTREQUEST = 1,
        DHCP_OP_BOOTREPLY = 2
    };

    /* TODO: Complete list from https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol#Options */
    enum DHCPOption
    {
        DHCP_OPTION_PAD = 0,
        DHCP_OPTION_SUBNETMASK = 1,
        DHCP_OPTION_TIME_OFFSET = 2,
        DHCP_OPTION_ROUTER = 3,
        DHCP_OPTION_TIME_SERVER = 4,
        DHCP_OPTION_NAME_SERVER = 5,
        DHCP_OPTION_DOMAIN_NAME_SERVER = 6,
        DHCP_OPTION_LOG_SERVER = 7,
        DHCP_OPTION_COOKIE_SERVER = 8,
        DHCP_OPTION_LPR_SERVER = 9,
        DHCP_OPTION_IMPRESS_SERVER = 10,
        DHCP_OPTION_RESOURCE_LOCATION_SERVER = 11,
        DHCP_OPTION_HOST_NAME = 12,
        DHCP_OPTION_BOOT_FILE_SIZE = 13,
        DHCP_OPTION_MERIT_DUMP_FILE = 14,
        DHCP_OPTION_DOMAIN_NAME = 15,
        DHCP_OPTION_SWAP_SERVER = 16,
        DHCP_OPTION_ROOT_PATH = 17,
        DHCP_OPTION_EXTENSION_PATH = 18,

        DHCP_OPTION_IP_FORWARDING = 19,
        DHCP_OPTION_NON_LOCAL_SOURCE_ROUTING = 20,
        DHCP_OPTION_POLICY_FILTER = 21,
        DHCP_OPTION_MAX_DATAGRAM_REASSEMBLY_SIZE = 22,
        DHCP_OPTION_DEFAULT_IP_TTL = 23,
        DHCP_OPTION_PATH_MTU_AGING_TIMEOUT = 24,
        DHCP_OPTION_PATH_MTU_PLATEAU_TABLE = 25,

        DHCP_OPTION_INTERFACE_MTU = 26,
        DHCP_OPTION_ALL_SUBNETS_ARE_LOCAL = 27,
        DHCP_OPTION_BROADCAST_ADDRESS = 28,
        DHCP_OPTION_PERFORM_MASK_DISCOVERY = 29,
        DHCP_OPTION_MASK_SUPPLIER = 30,
        DHCP_OPTION_ROUTER_DISCOVERY = 31,
        DHCP_OPTION_ROUTER_SOLICITATION_ADDRESS = 32,
        DHCP_OPTION_STATIC_ROUTE = 33,

        DHCP_OPTION_TRAILER_ENCAPSULATION = 34,
        DHCP_OPTION_ARP_CACHE_TIMEOUT = 35,
        DHCP_OPTION_ETHERNET_ENCAPSULATION = 36,

        DHCP_OPTION_DEFAULT_TCP_TTL = 37,
        DHCP_OPTION_TCP_KEEPALIVE_INTERVAL = 38,
        DHCP_OPTION_TCP_KEEPALIVE_GARBAGE = 39,

        DHCP_OPTION_NIS_DOMAIN = 40,
        DHCP_OPTION_NIS_SERVERS = 41,
        DHCP_OPTION_NTP_SERVERS = 42,
        DHCP_OPTION_VENDOR_SPECIFIC = 43,
        DHCP_OPTION_NETBIOS_NAME_SERVERS = 44,
        DHCP_OPTION_NETBIOS_DD_SERVER = 45,
        DHCP_OPTION_NETBIOS_NODE_TYPE = 46,
        DHCP_OPTION_NETBIOS_SCOPE = 47,
        DHCP_OPTION_X_FONT_SERVERS = 48,
        DHCP_OPTION_X_DISPLAY_MANAGER = 49,

        DHCP_OPTION_REQUESTED_IP = 50,
        DHCP_OPTION_IP_LEASE_TIME = 51,
        DHCP_OPTION_OPTION_OVERLOAD = 52,
        DHCP_OPTION_MESSAGE_TYPE = 53,
        DHCP_OPTION_SERVER_IDENTIFIER = 54,
        DHCP_OPTION_PARAMETER_REQUEST_LIST = 55,
        DHCP_OPTION_MESSAGE = 56,
        DHCP_OPTION_MAX_MESSAGE_SIZE = 57,
        DHCP_OPTION_T1_TIMEOUT = 58,
        DHCP_OPTION_T2_TIMEOUT = 59,
        DHCP_OPTION_VENDOR_CLASS_IDENTIFIER = 60,
        DHCP_OPTION_CLIENT_IDENTIFIER = 61,

        DHCP_OPTION_NETWORK_TIME_SERVER = 62,

        DHCP_OPTION_END = 255
    };

    enum DHCPMessageType
    {
        DHCP_MESSAGE_TYPE_DISCOVER = 1,
        DHCP_MESSAGE_TYPE_OFFER = 2,
        DHCP_MESSAGE_TYPE_REQUEST = 3,
        DHCP_MESSAGE_TYPE_DECLINE = 4,
        DHCP_MESSAGE_TYPE_ACK = 5,
        DHCP_MESSAGE_TYPE_NAK = 6,
        DHCP_MESSAGE_TYPE_RELEASE = 7,
        DHCP_MESSAGE_TYPE_INFORM = 8,
        DHCP_MESSAGE_TYPE_FORCERENEW = 9,
        DHCP_MESSAGE_TYPE_LEASEQUERY = 10,
        DHCP_MESSAGE_TYPE_LEASEUNASSIGNED = 11,
        DHCP_MESSAGE_TYPE_LEASEUNKNOWN = 12,
        DHCP_MESSAGE_TYPE_LEASEACTIVE = 13,
        DHCP_MESSAGE_TYPE_BULKLEASEQUERY = 14,
        DHCP_MESSAGE_TYPE_LEASEQUERYDONE = 15,
        DHCP_MESSAGE_TYPE_ACTIVELEASEQUERY = 16,
        DHCP_MESSAGE_TYPE_LEASEQUERYSTATUS = 17,
        DHCP_MESSAGE_TYPE_DHCPTLS = 18
    };

#define DHCP_TRANSACTION_ID 0xFE2EC005

    class DHCP : public NetworkUDP::UDPEvents
    {
    private:
        NetworkUDP::Socket *UDPSocket;
        NetworkInterfaceManager::DeviceInterface *Interface;
        bool Received = false;

        void CreatePacket(DHCPHeader *Packet, uint8_t MessageType, uint32_t RequestIP);
        void *GetOption(DHCPHeader *Packet, uint8_t Type);

    public:
        /** @brief IP address (Little-endian) */
        InternetProtocol4 IP = {.Address = {0x0, 0x0, 0x0, 0x0}};
        /** @brief Gateway address (Little-endian) */
        InternetProtocol4 Gateway = {.Address = {0x0, 0x0, 0x0, 0x0}};
        /** @brief Subnet mask (Little-endian) */
        InternetProtocol4 SubnetworkMask = {.Address = {0x0, 0x0, 0x0, 0x0}};
        /** @brief DNS server address (Little-endian) */
        InternetProtocol4 DomainNameSystem = {.Address = {0x0, 0x0, 0x0, 0x0}};

        DHCP(NetworkUDP::Socket *Socket, NetworkInterfaceManager::DeviceInterface *Interface);
        ~DHCP();
        void Request();
        void Request(InternetProtocol4 IP);

        virtual void OnUDPPacketReceived(NetworkUDP::Socket *Socket, uint8_t *Data, uint64_t Length);
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
    } __attribute__((packed));

    class NTP : public NetworkUDP::UDPEvents
    {
    private:
        NetworkUDP::Socket *Socket;

    public:
        NTP(NetworkUDP::Socket *Socket);
        ~NTP();

        void ReadTime();
    };
}

extern NetworkInterfaceManager::NetworkInterface *nimgr;
