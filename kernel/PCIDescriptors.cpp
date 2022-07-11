#include <types.h>
#include "libs/converter.h"

namespace PCI
{
    const char *DeviceClasses[]{
        "Unclassified",
        "Mass Storage Controller",
        "Network Controller",
        "Display Controller",
        "Multimedia Controller",
        "Memory Controller",
        "Bridge Device",
        "Simple Communication Controller",
        "Base System Peripheral",
        "Input Device Controller",
        "Docking Station",
        "Processor",
        "Serial Bus Controller",
        "Wireless Controller",
        "Intelligent Controller",
        "Satellite Communication Controller",
        "Encryption Controller",
        "Signal Processing Controller",
        "Processing Accelerator",
        "Non Essential Instrumentation"};

    enum PCIVendors
    {
        SymbiosLogic = 0x1000,
        RedHat = 0x1AF4,
        REDHat2 = 0x1B36,
        Realtek = 0x10EC,
        VirtualBox = 0x80EE,
        Ensoniq = 0x1274,
        QEMU = 0x1234,
        VMware = 0x15AD,
        IntelCorporation = 0x8086,
        AdvancedMicroDevices = 0x1022,
        NVIDIACorporation = 0x10DE
    };

    const char *MassStorageControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA";
        case 0x07:
            return "Serial Attached SCSI Controller";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Mass Storage Controller";
        }
        fixme("Unknown mass storage controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *NetworkControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "Ethernet Controller";
        case 0x01:
            return "Token Ring Controller";
        case 0x02:
            return "FDDI Controller";
        case 0x03:
            return "ATM Controller";
        case 0x04:
            return "ISDN Controller";
        case 0x05:
            return "WorldFip Controller";
        case 0x06:
            return "PICMG HyperCard Controller";
        case 0x07:
            return "Infiniband Controller";
        case 0x08:
            return "Fabric Controller";
        case 0x80:
            return "Network Controller";
        }
        fixme("Unknown network controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *DisplayControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "VGA Compatible Controller";
        case 0x01:
            return "XGA Controller";
        case 0x02:
            return "3D Controller";
        case 0x80:
            return "Display Controller";
        }
        fixme("Unknown display controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *CommunicationControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "Serial Controller";
        case 0x01:
            return "Parallel Controller";
        case 0x02:
            return "Multi-Serial Controller";
        case 0x03:
            return "IEEE-1284 Controller";
        case 0x04:
            return "ATM Controller";
        case 0x05:
            return "Object Storage Controller";
        case 0x80:
            return "Communication controller";
        }
        fixme("Unknown communication controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *BaseSystemPeripheralSubclassName(uint8_t SubclassCode)
    {
        // not sure if it's right
        switch (SubclassCode)
        {
        case 0x00:
            return "Unclassified";
        case 0x01:
            return "Keyboard";
        case 0x02:
            return "Pointing Device";
        case 0x03:
            return "Mouse";
        case 0x04:
            return "Scanner";
        case 0x05:
            return "Gameport";
        case 0x80:
            return "Unclassified";
        }
        fixme("Unknown base system peripheral %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *SerialBusControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus Controller";
        case 0x02:
            return "SSA Controller";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel Controller";
        case 0x05:
            return "SMBus Controller";
        case 0x06:
            return "Infiniband Controller";
        case 0x07:
            return "IPMI Interface Controller";
        case 0x08:
            return "SERCOS Interface (IEC 61491) Controller";
        case 0x09:
            return "CANbus Controller";
        case 0x80:
            return "Serial Bus Controller";
        }
        fixme("Unknown serial bus controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *BridgeDeviceSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge";
        case 0x0A:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Bridge Device";
        }
        fixme("Unknown bridge device %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *WirelessControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x11:
            return "Bluetooth";
        case 0x20:
            return "802.1a controller";
        case 0x21:
            return "802.1b controller";
        case 0x80:
            return "Wireless controller";
        }
        fixme("Unknown wireless controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *GetVendorName(uint32_t VendorID)
    {
        switch (VendorID)
        {
        case 0x1000:
            return "Symbios Logic";
        case 0x1B36:
        case 0x1AF4:
            return "Red Hat, Inc.";
        case 0x10EC:
            return "Realtek Semiconductor Co., Ltd.";
        case 0x80EE:
            return "VirtualBox";
        case 0x1274:
            return "Ensoniq";
        case 0x1234:
            return "QEMU";
        case 0x15AD:
            return "VMware";
        case 0x8086:
            return "Intel Corporation";
        case 0x1022:
            return "Advanced Micro Devices, Inc.";
        case 0x10DE:
            return "NVIDIA Corporation";
        case 0x1AE0:
            return "Google, Inc.";
        case 0x1a58:
            return "Razer USA Ltd.";
        case 0x1414:
            return "Microsoft Corporation";
        }
        fixme("Unknown vendor %04x", VendorID);
        return u32ToHexString(VendorID);
    }

    const char *GetDeviceName(uint32_t VendorID, uint32_t DeviceID)
    {
        switch (VendorID)
        {
        case SymbiosLogic:
            switch (DeviceID)
            {
            case 0x30:
                return "53c1030 PCI-X Fusion-MPT Dual Ultra320 SCSI";
            case 0x1000:
                return "63C815";
            }
            [[fallthrough]];
        case RedHat:
            switch (DeviceID)
            {
            case 0x1000:
            case 0x1041:
                return "Virtio network device";
            case 0x1001:
            case 0x1042:
                return "Virtio block device";
            case 0x1002:
            case 0x1045:
                return "Virtio memory balloon";
            case 0x1003:
            case 0x1043:
                return "Virtio console";
            case 0x1004:
            case 0x1048:
                return "Virtio SCSI";
            case 0x1005:
            case 0x1044:
                return "Virtio RNG";
            case 0x1009:
            case 0x1049:
            case 0x105a:
                return "Virtio filesystem";
            case 0x1050:
                return "Virtio GPU";
            case 0x1052:
                return "Virtio input";
            case 0x1053:
                return "Virtio socket";
            case 1110:
                return "Inter-VM shared memory";
            case 0x1af41100:
                return "QEMU Virtual Machine";
            }
            [[fallthrough]];
        case REDHat2:
            switch (DeviceID)
            {
            case 0x0001:
                return "QEMU PCI-PCI bridge";
            case 0x0002:
                return "QEMU PCI 16550A Adapter";
            case 0x0003:
                return "QEMU PCI Dual-port 16550A Adapter";
            case 0x0004:
                return "QEMU PCI Quad-port 16550A Adapter";
            case 0x0005:
                return "QEMU PCI Test Device";
            case 0x0006:
                return "PCI Rocker Ethernet switch device";
            case 0x0007:
                return "PCI SD Card Host Controller Interface";
            case 0x0008:
                return "QEMU PCIe Host bridge";
            case 0x0009:
                return "QEMU PCI Expander bridge";
            case 0x000A:
                return "PCI-PCI bridge (multiseat)";
            case 0x000B:
                return "QEMU PCIe Expander bridge";
            case 0x000C:
                return "QEMU PCIe Root port";
            case 0x000D:
                return "QEMU XHCI Host Controller";
            case 0x0010:
                return "QEMU NVM Express Controller";
            case 0x0100:
                return "QXL paravirtual graphic card";
            case 0x1AF41100:
                return "QEMU Virtual Machine";
            }
            [[fallthrough]];
        case Realtek:
            switch (DeviceID)
            {
            case 0x8029:
                return "RTL-8029(AS)";
            case 0x8139:
                return "RTL-8139/8139C/8139C+ Ethernet Controller";
            }
            [[fallthrough]];
        case VirtualBox:
            switch (DeviceID)
            {
            case 0xCAFE:
                return "VirtualBox Guest Service";
            case 0xBEEF:
                return "VirtualBox Graphics Adapter";
            case 0x0021:
                return "USB Tablet";
            case 0x0022:
                return "Multitouch tablet";
            case 0x4E56:
                return "NVM Express";
            }
            [[fallthrough]];
        case Ensoniq:
            switch (DeviceID)
            {
            case 0x5000:
                return "ES1370 [AudioPCI]";
            }
            [[fallthrough]];
        case QEMU:
            switch (DeviceID)
            {
            case 0x1111:
                return "QEMU Display";
            }
            [[fallthrough]];
        case VMware:
            switch (DeviceID)
            {
            case 0x0740:
                return "Virtual Machine Communication Interface";
            case 0x0405:
                return "SVGA II Adapter";
            case 0x0790:
                return "PCI bridge";
            case 0x07A0:
                return "PCI Express Root Port";
            case 0x0774:
                return "USB1.1 UHCI Controller";
            case 0x0770:
                return "USB2 EHCI Controller";
            case 0x0779:
                return "USB3 xHCI 1.0 Controller";
            case 0x07F0:
                return "NVM Express";
            }
            [[fallthrough]];
        case IntelCorporation:
            switch (DeviceID)
            {
            case 0x1229:
                return "82557/8/9/0/1 Ethernet Pro 100";
            case 0x1209:
                return "8255xER/82551IT Fast Ethernet Controller";
            case 0x100E:
                return "82540EM Gigabit Ethernet Controller";
            case 0x7190:
                return "440BX/ZX/DX - 82443BX/ZX/DX Host bridge";
            case 0x7191:
                return "440BX/ZX/DX - 82443BX/ZX/DX AGP bridge";
            case 0x7110:
                return "82371AB/EB/MB PIIX4 ISA";
            case 0x7111:
                return "82371AB/EB/MB PIIX4 IDE";
            case 0x7113:
                return "82371AB/EB/MB PIIX4 ACPI";
            case 0x1e31:
                return "7 Series/C210 Series Chipset Family USB xHCI Host Controller";
            case 0x100F:
                return "82545EM Gigabit Ethernet Controller (Copper)";
            case 0x1371:
                return "ES1371/ES1373 / Creative Labs CT2518";
            case 0x27b9:
                return "82801GBM (ICH7-M) LPC Interface Bridge";
            case 0x07E0:
                return "SATA AHCI controller";
            case 0x293E:
                return "82801I (ICH9 Family) HD Audio Controller";
            case 0x2935:
                return "82801I (ICH9 Family) USB UHCI Controller #2";
            case 0x2936:
                return "82801I (ICH9 Family) USB UHCI Controller #3";
            case 0x293A:
                return "82801I (ICH9 Family) USB2 EHCI Controller #1";
            case 0x2934:
                return "82801I (ICH9 Family) USB UHCI Controller #1";
            case 0x2668:
                return "82801FB/FBM/FR/FW/FRW (ICH6 Family) High Definition Audio Controller";
            case 0x2415:
                return "82801AA AC'97 Audio Controller";
            case 0x10D3:
                return "82574L Gigabit Network Connection";
            case 0x29C0:
                return "82G33/G31/P35/P31 Express DRAM Controller";
            case 0x2918:
                return "82801IB (ICH9) LPC Interface Controller";
            case 0x2829:
                return "82801HM/HEM (ICH8M/ICH8M-E) SATA Controller [AHCI mode]";
            case 0x2922:
                return "82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]";
            case 0x2930:
                return "82801I (ICH9 Family) SMBus Controller";
            }
            [[fallthrough]];
        case AdvancedMicroDevices:
            switch (DeviceID)
            {
            case 0x2000:
                return "79C970 [PCnet32 LANCE]";
            }
        }
        fixme("Unknown device %04x:%04x", VendorID, DeviceID);
        return u32ToHexString(DeviceID);
    }

    const char *GetSubclassName(uint8_t ClassCode, uint8_t SubclassCode)
    {
        switch (ClassCode)
        {
        case 0x00:
            return "Unclassified";
        case 0x01:
            return MassStorageControllerSubclassName(SubclassCode);
        case 0x02:
            return NetworkControllerSubclassName(SubclassCode);
        case 0x03:
            return DisplayControllerSubclassName(SubclassCode);
        case 0x04:
            return "Multimedia controller";
        case 0x05:
            return "Memory Controller";
        case 0x06:
            return BridgeDeviceSubclassName(SubclassCode);
        case 0x07:
            return CommunicationControllerSubclassName(SubclassCode);
        case 0x08:
            return BaseSystemPeripheralSubclassName(SubclassCode);
        case 0x09:
            return "Input device controller";
        case 0x0A:
            return "Docking station";
        case 0x0B:
            return "Processor";
        case 0x0C:
            return SerialBusControllerSubclassName(SubclassCode);
        case 0x0D:
            return WirelessControllerSubclassName(SubclassCode);
        case 0x0E:
            return "Intelligent controller";
        case 0x0F:
            return "Satellite communication controller";
        case 0x10:
            return "Encryption controller";
        case 0x11:
            return "Signal processing accelerators";
        case 0x12:
            return "Processing accelerators";
        case 0x13:
            return "Non-Essential Instrumentation";
        case 0x40:
            return "Coprocessor";
        }
        fixme("Unknown subclass name %02x:%02x", ClassCode, SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *GetProgIFName(uint8_t ClassCode, uint8_t SubclassCode, uint8_t ProgIF)
    {
        switch (ClassCode)
        {
        case 0x01:
        {
            switch (SubclassCode)
            {
            case 0x06:
            {
                switch (ProgIF)
                {
                case 0:
                    return "Vendor Specific SATA Controller";
                case 1:
                    return "AHCI SATA Controller";
                case 2:
                    return "Serial Storage Bus SATA Controller";
                default:
                    return "SATA controller";
                }
            }
            case 0x08:
            {
                switch (ProgIF)
                {
                case 0x01:
                    return "NVMHCI Controller";
                case 0x02:
                    return "NVM Express Controller";
                default:
                    return "Non-Volatile Memory Controller";
                }
            }
            }
        }
        case 0x03:
        {
            switch (SubclassCode)
            {
            case 0x00:
                switch (ProgIF)
                {
                case 0x00:
                    return "VGA Controller";
                case 0x01:
                    return "8514-Compatible Controller";
                default:
                    return "VGA Compatible Controller";
                }
            }
        }
        case 0x07:
        {
            switch (SubclassCode)
            {
            case 0x00:
            {
                switch (ProgIF)
                {
                case 0x00:
                    return "Serial controller <8250>";
                case 0x01:
                    return "Serial controller <16450>";
                case 0x02:
                    return "Serial controller <16550>";
                case 0x03:
                    return "Serial controller <16650>";
                case 0x04:
                    return "Serial controller <16750>";
                case 0x05:
                    return "Serial controller <16850>";
                case 0x06:
                    return "Serial controller <16950";
                default:
                    return "Serial controller";
                }
            }
            }
        }
        case 0x0C:
        {
            switch (SubclassCode)
            {
            case 0x00:
                switch (ProgIF)
                {
                case 0x00:
                    return "Generic FireWire (IEEE 1394) Controller";
                case 0x10:
                    return "OHCI FireWire (IEEE 1394) Controller";
                }
            case 0x03:
                switch (ProgIF)
                {
                case 0x00:
                    return "UHCI (USB1) Controller";
                case 0x10:
                    return "OHCI (USB1) Controller";
                case 0x20:
                    return "EHCI (USB2) Controller";
                case 0x30:
                    return "XHCI (USB3) Controller";
                case 0x80:
                    return "Unspecified";
                case 0xFE:
                    return "USB Device";
                }
            }
        }
        }
        // not really a fixme
        // fixme("Unknown prog IF name %02x:%02x:%02x", ClassCode, SubclassCode, ProgIF);
        return u8ToHexString(ProgIF);
    }
}
