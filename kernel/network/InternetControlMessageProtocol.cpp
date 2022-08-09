#include "NetworkController.hpp"

namespace NetworkICMPv4
{
    ICMPv4::ICMPv4(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }
    ICMPv4::~ICMPv4() {}

    void ICMPv4::Send(/* ???? */)
    {
        fixme("Unimplemented");
    }

    void ICMPv4::Receive(ICMPPacket *Packet)
    {
        if (Packet->Header.Type == ICMPv4Type::TYPE_ECHO)
        {
            // TODO: This probably doesn't work
            netdbg("ICMPv4: Echo Request");
            Packet->Header.Type = ICMPv4Type::TYPE_ECHO_REPLY;
            Packet->Header.Code = 0x0;
            Packet->Header.Checksum = CalculateChecksum((uint16_t *)Packet, sizeof(ICMPHeader));
            this->Interface->Send((uint8_t *)Packet, sizeof(ICMPHeader) + 0);
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

    void ICMPv6::Send(uint8_t *Data, uint64_t Length) { fixme("Unimplemented"); }
    void ICMPv6::Receive(uint8_t *Data) { fixme("Unimplemented"); }
}
