#include "AMDPCNET.hpp"

/* https://wiki.osdev.org/AMD_PCNET */

namespace AMDPCNET
{
    MediaAccessControl NetworkInterfaceController::GetMAC()
    {
        return MediaAccessControl();
    }

    InternetProtocol NetworkInterfaceController::GetIP()
    {
        return IP;
    }

    void NetworkInterfaceController::SetIP(InternetProtocol IP)
    {
        this->IP = IP;
    }

    NetworkInterfaceController::NetworkInterfaceController(PCI::PCIDeviceHeader *PCIBaseAddress, int ID)
    {
        if (PCIBaseAddress->VendorID != 0x10EC || PCIBaseAddress->DeviceID != 0x8169)
        {
            netdbg("Not a AMD PCNET network card");
            return;
        }
        netdbg("Found %s network card", PCI::GetDeviceName(PCIBaseAddress->VendorID, PCIBaseAddress->DeviceID));
        uint32_t PCIBAR = ((PCI::PCIHeader0 *)PCIBaseAddress)->BAR0;
        BAR.Type = PCIBAR & 1;
        BAR.IOBase = PCIBAR & (~3);
        BAR.MemoryBase = PCIBAR & (~15);
        netdbg("BAR Type: %d - BAR IOBase: %#x - BAR MemoryBase: %#x", BAR.Type, BAR.IOBase, BAR.MemoryBase);
    }

    NetworkInterfaceController::~NetworkInterfaceController()
    {
    }

    void NetworkInterfaceController::Send(void *Data, uint64_t Length)
    {
    }

    void NetworkInterfaceController::Receive()
    {
    }

    void NetworkInterfaceController::AMDPCNETInterruptHandler()
    {
    }
}
