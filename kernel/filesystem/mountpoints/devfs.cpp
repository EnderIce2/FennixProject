#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *DeviceRootNode = nullptr;
    static uint64_t DeviceNodeIndexNodeCount = 0;

    FileSystemNode *Device::AddFileSystem(FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
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
        newNode->IndexNode = DeviceNodeIndexNodeCount++;
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        DeviceRootNode->Children.push_back(newNode);
        return newNode;
    }

    Device::Device()
    {
        trace("Initializing device file system...");
        DeviceRootNode = vfs->Create(nullptr, "/system/dev");
        DeviceRootNode->Flags = NodeFlags::FS_DIRECTORY;
        DeviceRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Device::~Device()
    {
    }
}
