#include "NetworkController.hpp"

/* https://en.wikipedia.org/wiki/Ethernet_frame */

namespace NetworkEthernetFrame
{
    EthernetFrame::EthernetFrame(NetworkInterfaceManager::DeviceInterface *Interface)
    {
        this->Interface = Interface;
    }

    EthernetFrame::~EthernetFrame()
    {
    }

    void EthernetFrame::Send(MediaAccessControl MAC, FrameType Type, void *Data, uint64_t Length)
    {
    }

    void EthernetFrame::Receive(void *Data, uint64_t Length)
    {
    }
}
