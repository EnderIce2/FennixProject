#include "NetworkController.hpp"

#include <int.h>

#include "../drivers/network/RTL8139/RTL8139.hpp"
#include "../drivers/network/RTL8169/RTL8169.hpp"
#include "../drivers/network/E1000/E1000.hpp"
#include "../pci.h"

NetworkInterfaceManager::NetworkInterface *nimgr = nullptr;

RTL8139::NetworkInterfaceController *rtl8139 = nullptr;
RTL8169::NetworkInterfaceController *rtl8169 = nullptr;
E1000::NetworkInterfaceController *e1000 = nullptr;

InterruptHandler(E1000StubInterruptHandler)
{
    e1000->E1000InterruptHandler(regs);
}

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

        // TODO: initialize more cards at once
        if (!rtl8139)
            foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8139))
                rtl8139 = new RTL8139::NetworkInterfaceController(PCIData, CardIDs++);

        if (!rtl8169)
            foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8169))
                rtl8169 = new RTL8169::NetworkInterfaceController(PCIData, CardIDs++);

        if (!e1000)
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100E))
            {
                e1000 = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
                if (!ValidMAC(e1000->GetMAC()))
                {
                    delete e1000;
                    err("E1000: Invalid MAC address!");
                }
                break;
            }

        if (!e1000)
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x153A))
            {
                e1000 = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
                if (!ValidMAC(e1000->GetMAC()))
                {
                    delete e1000;
                    err("E1000: Invalid MAC address!");
                }
                break;
            }

        if (!e1000)
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x10EA))
            {
                e1000 = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
                if (!ValidMAC(e1000->GetMAC()))
                {
                    delete e1000;
                    err("E1000: Invalid MAC address!");
                }
                break;
            }

        if (!e1000)
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x109A))
            {
                e1000 = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
                if (!ValidMAC(e1000->GetMAC()))
                {
                    delete e1000;
                    err("E1000: Invalid MAC address!");
                }
                break;
            }

        if (!e1000)
            foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100F))
            {
                e1000 = new E1000::NetworkInterfaceController(PCIData, CardIDs++);
                if (!ValidMAC(e1000->GetMAC()))
                {
                    delete e1000;
                    err("E1000: Invalid MAC address!");
                }
                break;
            }

        if (e1000)
        {
            RegisterInterrupt(E1000StubInterruptHandler, IRQ11, true);
            e1000->Start();
            MediaAccessControl mac = e1000->GetMAC();
            if (!ValidMAC(mac))
            {
                err("E1000: MAC address is invalid %x:%x:%x:%x:%x:%x",
                    mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
                delete e1000;
            }
            else
                netdbg("E1000: MAC: %02x:%02x:%02x:%02x:%02x:%02x",
                       mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
        }
    }

    NetworkInterface::~NetworkInterface()
    {
    }

    void NetworkInterface::Scan()
    {
        fixme("Scan for network interfaces");
    }

    void NetworkInterface::Send(DeviceInterface Interface, void *Data, int Length) { Interface.Send(Data, Length); }
    void NetworkInterface::Receive(DeviceInterface Interface, void *Data, int Length) { Interface.Receive(Data); }
}
