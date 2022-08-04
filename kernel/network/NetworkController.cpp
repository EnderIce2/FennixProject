#include "NetworkController.hpp"

#include <int.h>

#include "../drivers/network/RTL8139/RTL8139.hpp"
#include "../drivers/network/RTL8169/RTL8169.hpp"
#include "../drivers/network/E1000/E1000.hpp"
#include "../pci.h"

// TODO: warning: deleting object of polymorphic class type ‘X::NetworkInterfaceController’ which has non-virtual destructor might cause undefined behavior

NetworkInterfaceManager::NetworkInterface *nimgr = nullptr;

RTL8139::NetworkInterfaceController *rtl8139[8];
int rtl8139Count = 0;

RTL8169::NetworkInterfaceController *rtl8169[8];
int rtl8169Count = 0;

E1000::NetworkInterfaceController *e1000[8];
int e1000Count = 0;

InterruptHandler(E1000StubInterruptHandler)
{
    // TODO: support more
    fixme("E1000 stub interrupt: Received IRQ%d", regs->int_num - 32);
    e1000[0]->E1000InterruptHandler(regs);
}

void TraceCards()
{
    int rtl8139CardCount = 0;
    foreach (auto card in rtl8139)
    {
        if (card)
        {
            MediaAccessControl mac = card->GetMAC();
            if (!ValidMAC(mac))
            {
                err("RTL-8139[%d]: MAC address is invalid %x:%x:%x:%x:%x:%x", rtl8139CardCount,
                    mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
            }
            else
                netdbg("RTL-8139[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8139CardCount,
                       mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
        }
        else
            err("RTL-8139[%d]: Card not found", rtl8139CardCount);
        rtl8139CardCount++;
    }

    int rtl8169CardCount = 0;
    foreach (auto card in rtl8169)
    {
        if (card)
        {
            MediaAccessControl mac = card->GetMAC();
            if (!ValidMAC(mac))
            {
                err("RTL-8169[%d]: MAC address is invalid %x:%x:%x:%x:%x:%x", rtl8169CardCount,
                    mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
            }
            else
                netdbg("RTL-8169[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", rtl8169CardCount,
                       mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
        }
        else
            err("RTL-8169[%d]: Card not found", rtl8169CardCount);
        rtl8169CardCount++;
    }

    int e1000CardCount = 0;
    foreach (auto card in e1000)
    {
        if (card)
        {
            MediaAccessControl mac = card->GetMAC();
            if (!ValidMAC(mac))
            {
                err("E1000[%d]: MAC address is invalid %x:%x:%x:%x:%x:%x", e1000CardCount,
                    mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
            }
            else
                netdbg("E1000[%d]: MAC: %02x:%02x:%02x:%02x:%02x:%02x", e1000CardCount,
                       mac.Address[0], mac.Address[1], mac.Address[2], mac.Address[3], mac.Address[4], mac.Address[5]);
        }
        else
            err("E1000[%d]: Card not found", e1000CardCount);
        e1000CardCount++;
    }
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
        foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8139))
            rtl8139[rtl8139Count++] = new RTL8139::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x10EC, 0x8169))
            rtl8169[rtl8169Count++] = new RTL8169::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100E))
            e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x153A))
            e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x10EA))
            e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x109A))
            e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

        foreach (auto PCIData in PCI::FindPCIDevice(0x8086, 0x100F))
            e1000[e1000Count++] = new E1000::NetworkInterfaceController(PCIData, CardIDs++);

        if (e1000[0])
        {
            /* FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME */
            RegisterInterrupt(E1000StubInterruptHandler, IRQ10, true); // TODO: ASSIGN INTERRUPT PROPERLY!
            RegisterInterrupt(E1000StubInterruptHandler, IRQ11, true); // TODO: ASSIGN INTERRUPT PROPERLY!
            e1000[0]->Start();
        }

        TraceCards();
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
