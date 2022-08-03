#include "NetworkController.hpp"

#include "../pci.h"

struct NetworkCards
{
    char Name[16];
    uint16_t VendorID;
    uint16_t DeviceID;
};

// TODO: Parse a config file with the data
NetworkCards NetCards[8] = {
    "RTL-8139", 0x10EC, 0x8139,
    "RTL-8169", 0x10EC, 0x8169,
    "E1000", 0x8086, 0x100E,
    "E1000", 0x8086, 0x153A,
    "E1000", 0x8086, 0x10EA,
    "E1000", 0x8086, 0x109A,
    "E1000", 0x8086, 0x100F,
    "AMD PCNET", 0x1022, 0x2000};

NetworkInterfaceManager::NetworkInterface *nimgr = nullptr;

namespace NetworkInterfaceManager
{
    NetworkInterface::NetworkInterface()
    {
        foreach (auto Card in NetCards)
        {
            netdbg("Searching for network card %s [%#lx:%#lx]", Card.Name, Card.VendorID, Card.DeviceID);
            foreach (auto CardData in PCI::FindPCIDevice(Card.VendorID, Card.DeviceID))
            {
                trace("Found %s with status %ld", Card.Name, CardData->Status);
            }
        }
    }

    NetworkInterface::~NetworkInterface()
    {
    }

    void NetworkInterface::Scan()
    {
        fixme("Scan for network interfaces");
    }

    void NetworkInterface::Send(DeviceInterface Interface, void *Data, int Length)
    {
    }

    void NetworkInterface::Receive(DeviceInterface Interface, void *Data, int Length)
    {
    }
}
