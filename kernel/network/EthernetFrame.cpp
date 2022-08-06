#include "NetworkController.hpp"

#include <heap.h>

/* https://en.wikipedia.org/wiki/Ethernet_frame */

namespace NetworkEthernetFrame
{
    EthernetFrame::EthernetFrame(NetworkInterfaceManager::DeviceInterface *Interface) { this->Interface = Interface; }

    EthernetFrame::~EthernetFrame()
    {
        netdbg("EthernetFrame::~EthernetFrame()");
    }

    void EthernetFrame::Send(MediaAccessControl MAC, FrameType Type, void *Data, uint64_t Length)
    {
        void *Packet = KernelAllocator.RequestPages(sizeof(EthernetFrameHeader) / PAGE_SIZE + 1);
        /* Not sure if this will convert it correctly */
        ((EthernetFrameHeader *)Packet)->SourceMAC = this->Interface->MAC.raw;
        ((EthernetFrameHeader *)Packet)->DestinationMAC = MAC.raw;
        ((EthernetFrameHeader *)Packet)->Type = Type;

        memcpy((void *)((uint64_t)Packet + sizeof(EthernetFrameHeader)), Data, Length);
        this->Interface->Send(Packet, Length + sizeof(EthernetFrameHeader));
        KernelAllocator.FreePages(Packet, sizeof(EthernetFrameHeader) / PAGE_SIZE + 1);
    }

    void EthernetFrame::Receive(void *Data, uint64_t Length)
    {
    }
}
