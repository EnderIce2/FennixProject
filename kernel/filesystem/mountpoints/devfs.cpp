#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *DeviceRootNode = nullptr;

    FileSystemNode *Device::AddFileSystem(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to device file system", Name);
        // char *FullPath = new char[256];
        // strcpy(FullPath, "/system/dev");
        // strcat(FullPath, Name);
        FileSystemNode *newNode = /* vfs->Create(nullptr, "/system/dev"); */ vfs->Create(DeviceRootNode, Name);
        // delete[] FullPath;
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;

#ifdef DEBUG_FILESYSTEM
        foreach (auto var in DeviceRootNode->Children)
            vfsdbg("Has [DeviceRootNode]: %s (should have also %s)", var->Name, Name);

        foreach (auto var in newNode->Children)
            vfsdbg("Has [newNode]: %s (should have also %s)", var->Name, Name);
#endif
        return newNode;
    }

    Device::Device()
    {
        trace("Initializing device file system...");
        DeviceRootNode = vfs->Create(nullptr, "/system/dev");
        DeviceRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        DeviceRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Device::~Device()
    {
        warn("Tried to uninitialize Device File System!");
    }
}
