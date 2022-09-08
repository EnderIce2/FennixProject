#include <filesystem.h>

#include <bootscreen.h>
#include <display.h>
#include <heap.h>
#include <asm.h>

#include "../../cpu/cpuid.h"

namespace FileSystem
{
    FileSystemNode *SysInfoRootNode = nullptr;
    static uint64_t SysInfoNodeIndexNodeCount = 0;

    FileSystemNode *SysInfo::AddInfo(FileSystemOpeations *Operator, string Name)
    {
        trace("Adding %s to device file system", Name);
        // char *FullPath = new char[256];
        // strcpy(FullPath, "/system/dev/");
        // strcat(FullPath, Name);
        // FileSystemNode *newNode = vfs->Create(nullptr, FullPath);
        // delete[] FullPath;

        // FileSystemNode *newNode = vfs->Create(DeviceRootNode, Name);

        FileSystemNode *newNode = new FileSystemNode;
        strcpy(newNode->Name, Name);
        newNode->IndexNode = SysInfoNodeIndexNodeCount++;
        newNode->Mode = 0444;
        newNode->Operator = Operator;
        newNode->Flags = NodeFlags::FS_FILE;
        SysInfoRootNode->Children.push_back(newNode);
        return newNode;
    }

    ReadFSFunction(FB_Address_Read)
    {
        uint32_t ret = CurrentDisplay->GetFramebuffer()->Address;
        Buffer = (uint8_t *)((uint64_t)ret);
        return ret;
    }
    FileSystemOpeations sysinfo_fb_address = {.Name = "SysInfo Data", .Read = FB_Address_Read};

    ReadFSFunction(FB_Width_Read)
    {
        uint32_t ret = CurrentDisplay->GetFramebuffer()->Width;
        Buffer = (uint8_t *)((uint64_t)ret);
        return ret;
    }
    FileSystemOpeations sysinfo_fb_width = {.Name = "SysInfo Data", .Read = FB_Width_Read};

    ReadFSFunction(FB_Height_Read)
    {
        uint32_t ret = CurrentDisplay->GetFramebuffer()->Height;
        Buffer = (uint8_t *)((uint64_t)ret);
        return ret;
    }
    FileSystemOpeations sysinfo_fb_height = {.Name = "SysInfo Data", .Read = FB_Height_Read};

    ReadFSFunction(FB_PixelsPerScanLine_Read)
    {
        uint32_t ret = CurrentDisplay->GetFramebuffer()->PixelsPerScanLine;
        Buffer = (uint8_t *)((uint64_t)ret);
        return ret;
    }
    FileSystemOpeations sysinfo_fb_ppsl = {.Name = "SysInfo Data", .Read = FB_PixelsPerScanLine_Read};

    ReadFSFunction(FB_Size_Read)
    {
        uint32_t ret = CurrentDisplay->GetFramebuffer()->Size;
        Buffer = (uint8_t *)((uint64_t)ret);
        return ret;
    }
    FileSystemOpeations sysinfo_fb_size = {.Name = "SysInfo Data", .Read = FB_Size_Read};

    ReadFSFunction(CPU_Vendor_Read)
    {
        cpuid_string(0, (int *)(Buffer));
        return strlen((char *)Buffer);
    }
    FileSystemOpeations sysinfo_cpu_vendor = {.Name = "SysInfo Data", .Read = CPU_Vendor_Read};

    ReadFSFunction(CPU_Name_Read)
    {
        uint32_t rax, rbx, rcx, rdx;
        uint32_t func;
        cpuid(0x80000000, &func, &rbx, &rcx, &rdx);
        if (func >= 0x80000004)
        {
            char name_cpuid[48];
            cpuid(0x80000002, (uint32_t *)(name_cpuid + 0), (uint32_t *)(name_cpuid + 4), (uint32_t *)(name_cpuid + 8), (uint32_t *)(name_cpuid + 12));
            cpuid(0x80000003, (uint32_t *)(name_cpuid + 16), (uint32_t *)(name_cpuid + 20), (uint32_t *)(name_cpuid + 24), (uint32_t *)(name_cpuid + 28));
            cpuid(0x80000004, (uint32_t *)(name_cpuid + 32), (uint32_t *)(name_cpuid + 36), (uint32_t *)(name_cpuid + 40), (uint32_t *)(name_cpuid + 44));
            const char *space = name_cpuid;
            while (*space == ' ')
            {
                ++space;
            }
            memcpy(Buffer, name_cpuid, strlen(name_cpuid));
        }
        else
        {
            memcpy(Buffer, "unknown", 7);
        }
        return strlen((char *)Buffer);
    }
    FileSystemOpeations sysinfo_cpu_name = {.Name = "SysInfo Data", .Read = CPU_Name_Read};

    ReadFSFunction(CPU_Hyper_Read)
    {
        uint32_t rax, rbx, rcx, rdx;
        char vendor[12];
        cpuid(0x40000000, &rax, &rbx, &rcx, &rdx);
        memcpy(vendor + 0, &rbx, 4);
        memcpy(vendor + 4, &rcx, 4);
        memcpy(vendor + 8, &rdx, 4);
        memcpy(Buffer, vendor, strlen(vendor));
        return strlen(vendor);
    }
    FileSystemOpeations sysinfo_cpu_hypervisor = {.Name = "SysInfo Data", .Read = CPU_Hyper_Read};

    ReadFSFunction(RAM_Used_Read)
    {
        sprintf_((char *)Buffer, "%ld", KernelAllocator.GetUsedRAM());
        return 0;
    }
    FileSystemOpeations sysinfo_mem_used = {.Name = "SysInfo Data", .Read = RAM_Used_Read};

    ReadFSFunction(RAM_Free_Read)
    {
        sprintf_((char *)Buffer, "%ld", KernelAllocator.GetFreeRAM());
        return 0;
    }
    FileSystemOpeations sysinfo_mem_free = {.Name = "SysInfo Data", .Read = RAM_Free_Read};

    ReadFSFunction(RAM_Total_Read)
    {
        sprintf_((char *)Buffer, "%ld", KernelAllocator.GetTotalRAM());
        return 0;
    }
    FileSystemOpeations sysinfo_mem_total = {.Name = "SysInfo Data", .Read = RAM_Total_Read};

    ReadFSFunction(RAM_Reserved_Read)
    {
        sprintf_((char *)Buffer, "%ld", KernelAllocator.GetReservedRAM());
        return 0;
    }
    FileSystemOpeations sysinfo_mem_reserved = {.Name = "SysInfo Data", .Read = RAM_Reserved_Read};

    ReadFSFunction(KERNEL_VERSION_Read)
    {
        sprintf_((char *)Buffer, "%s", KERNEL_VERSION);
        return 0;
    }
    FileSystemOpeations sysinfo_kernel_version = {.Name = "SysInfo Data", .Read = KERNEL_VERSION_Read};

    ReadFSFunction(KERNEL_NAME_Read)
    {
        sprintf_((char *)Buffer, "%s", KERNEL_NAME);
        return 0;
    }
    FileSystemOpeations sysinfo_kernel_name = {.Name = "SysInfo Data", .Read = KERNEL_NAME_Read};

    ReadFSFunction(KERNEL_BUILD_DATE_Read)
    {
        sprintf_((char *)Buffer, "%s %s", __DATE__, __TIME__);
        return 0;
    }
    FileSystemOpeations sysinfo_kernel_build_date = {.Name = "SysInfo Data", .Read = KERNEL_BUILD_DATE_Read};

    SysInfo::SysInfo()
    {
        trace("Initializing device file system...");
        SysInfoRootNode = vfs->Create(nullptr, "/system/sys");
        SysInfoRootNode->Flags = NodeFlags::FS_DIRECTORY;
        SysInfoRootNode->Mode = 0755;
        this->AddInfo(&sysinfo_fb_address, "fb0address");
        this->AddInfo(&sysinfo_fb_width, "fb0width");
        this->AddInfo(&sysinfo_fb_height, "fb0height");
        this->AddInfo(&sysinfo_fb_ppsl, "fb0ppsl");
        this->AddInfo(&sysinfo_fb_size, "fb0size");

        this->AddInfo(&sysinfo_cpu_vendor, "cpu_vendor");
        this->AddInfo(&sysinfo_cpu_name, "cpu_name");
        this->AddInfo(&sysinfo_cpu_hypervisor, "cpu_hypervisor");

        this->AddInfo(&sysinfo_mem_used, "mem_used");
        this->AddInfo(&sysinfo_mem_free, "mem_free");
        this->AddInfo(&sysinfo_mem_total, "mem_total");
        this->AddInfo(&sysinfo_mem_reserved, "mem_reserved");

        this->AddInfo(&sysinfo_kernel_version, "kernel_version");
        this->AddInfo(&sysinfo_kernel_name, "kernel_name");
        this->AddInfo(&sysinfo_kernel_build_date, "kernel_build_date");

        BS->IncreaseProgres();
    }

    SysInfo::~SysInfo()
    {
    }
}
