#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *DriverRootNode = nullptr;

    FileSystemNode *Driver::AddDriver(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        FileSystemNode *newNode = vfs->Create(DriverRootNode, Name);
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        return newNode;
    }

    Driver::Driver()
    {
        trace("Initializing driver file system...");
        DriverRootNode = vfs->Create(nullptr, "/system/drv");
        DriverRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        DriverRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Driver::~Driver()
    {
    }
}
