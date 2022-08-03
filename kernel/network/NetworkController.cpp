#include "NetworkController.hpp"

#include "../drivers/network/RTL8139/RTL8139.hpp"
#include "../pci.h"

NetworkInterfaceManager::NetworkInterface *nimgr = nullptr;

RTL8139::NetworkInterfaceController *rtl8139 = nullptr;

namespace NetworkInterfaceManager
{
    NetworkInterface::NetworkInterface()
    {
        // "RTL-8139", 0x10EC, 0x8139,
        // "RTL-8169", 0x10EC, 0x8169,
        // "E1000", 0x8086, 0x100E,
        // "E1000", 0x8086, 0x153A,
        // "E1000", 0x8086, 0x10EA,
        // "E1000", 0x8086, 0x109A,
        // "E1000", 0x8086, 0x100F,
        // "AMD PCNET", 0x1022, 0x2000

        foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8139))
            rtl8139 = new RTL8139::NetworkInterfaceController(PCIData, CardIDs++);
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
