#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *DriverRootNode = nullptr;
    static uint64_t DriverNodeIndexNodeCount = 0;

    FileSystemNode *Driver::AddDriver(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        // FileSystemNode *newNode = vfs->Create(DriverRootNode, Name);
        FileSystemNode *newNode = new FileSystemNode;
        strcpy(newNode->Name, Name);
        newNode->IndexNode = DriverNodeIndexNodeCount++;
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        DriverRootNode->Children.push_back(newNode);
        return newNode;
    }

    Driver::Driver()
    {
        trace("Initializing driver file system...");
        DriverRootNode = vfs->Create(nullptr, "/system/drv");
        DriverRootNode->Flags = NodeFlags::FS_DIRECTORY;
        DriverRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Driver::~Driver()
    {
    }
}
