#include "NetworkController.hpp"

namespace NetworkICMPv4
{
    ICMPv4::ICMPv4(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }
    ICMPv4::~ICMPv4() {}

    void ICMPv4::Send(/* ???? */)
    {
        fixme("Unimplemented");
    }

    void ICMPv4::Receive(ICMPPacket *Packet, NetworkIPv4::IPv4Packet *IPv4Packet)
    {
        if (Packet->Header.Type == ICMPv4Type::TYPE_ECHO)
        {
            // TODO: This probably doesn't work
            netdbg("ICMPv4: Echo Request");
            Packet->Header.Type = ICMPv4Type::TYPE_ECHO_REPLY;
            Packet->Header.Code = 0x0;
            Packet->Header.Checksum = CalculateChecksum(Packet, sizeof(ICMPHeader));
            this->Interface->Send(Packet, sizeof(ICMPHeader) + IPv4Packet->Header.TotalLength);
        }
        else
        {
            netdbg("ICMPv4: Unknown type %d", Packet->Header.Type);
        }
    }
}

namespace NetworkICMPv6
{
    ICMPv6::ICMPv6(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }
    ICMPv6::~ICMPv6() {}

    void ICMPv6::Send(void *Data, int Length) { fixme("Unimplemented"); }
    void ICMPv6::Receive(void *Data) { fixme("Unimplemented"); }
}
