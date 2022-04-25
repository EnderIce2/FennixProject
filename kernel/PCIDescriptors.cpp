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
        RedHat = 0x1AF4,
        Realtek = 0x10EC,
        VirtualBox = 0x80EE,
        Ensoniq = 0x1274,
        QEMU = 0x1234,
        VMware = 0x15AD,
        IntelCorporation = 0x8086,
        AdvancedMicroDevices = 0x1022,
        NVIDIACorporation = 0x10DE
    };

    const char *GetVendorName(uint16_t VendorID)
    {
        switch (VendorID)
        {
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
        fixme("Unkown vendor %04x", VendorID);
        return u16ToHexString(VendorID);
    }

    const char *GetDeviceName(uint16_t VendorID, uint16_t DeviceID)
    {
        switch (VendorID)
        {
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
        return u16ToHexString(DeviceID);
    }

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
            return "Serial Attached SCSI";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other (msc)";
        }
        fixme("Unknown mass storage controller %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *SerialBusControllerSubclassName(uint8_t SubclassCode)
    {
        switch (SubclassCode)
        {
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus";
        case 0x06:
            return "Infiniband";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus";
        case 0x80:
            return "Other (sbc)";
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
        case 0x0a:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other (bds)";
        }
        fixme("Unknown bridge device %02x", SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *GetSubclassName(uint8_t ClassCode, uint8_t SubclassCode)
    {
        switch (ClassCode)
        {
        case 0x01:
            return MassStorageControllerSubclassName(SubclassCode);
        case 0x03:
            switch (SubclassCode)
            {
            case 0x00:
                return "VGA Compatible Controller";
            }
            [[fallthrough]];
        case 0x06:
            return BridgeDeviceSubclassName(SubclassCode);
        case 0x0C:
            return SerialBusControllerSubclassName(SubclassCode);
        }
        fixme("Unknown subclass name %02x:%02x", ClassCode, SubclassCode);
        return u8ToHexString(SubclassCode);
    }

    const char *GetProgIFName(uint8_t ClassCode, uint8_t SubclassCode, uint8_t ProgIF)
    {
        switch (ClassCode)
        {
        case 0x01:
            switch (SubclassCode)
            {
            case 0x06:
                switch (ProgIF)
                {
                case 0x00:
                    return "Vendor Specific Interface";
                case 0x01:
                    return "AHCI 1.0";
                case 0x02:
                    return "Serial Storage Bus";
                }
            }
            [[fallthrough]];
        case 0x03:
            switch (SubclassCode)
            {
            case 0x00:
                switch (ProgIF)
                {
                case 0x00:
                    return "VGA Controller";
                case 0x01:
                    return "8514-Compatible Controller";
                }
            }
            [[fallthrough]];
        case 0x0C:
            switch (SubclassCode)
            {
            case 0x03:
                switch (ProgIF)
                {
                case 0x00:
                    return "UHCI Controller";
                case 0x10:
                    return "OHCI Controller";
                case 0x20:
                    return "EHCI (USB2) Controller";
                case 0x30:
                    return "XHCI (USB3) Controller";
                case 0x80:
                    return "Unspecified";
                case 0xFE:
                    return "USB Device (Not a Host Controller)";
                }
            }
        }
        fixme("Unknown prog IF name %02x:%02x:%02x", ClassCode, SubclassCode, ProgIF);
        return u8ToHexString(ProgIF);
    }
}