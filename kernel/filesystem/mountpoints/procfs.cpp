#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *ProcessRootNode = nullptr;

    Process::Process()
    {
        trace("Initializing process file system");
        ProcessRootNode = vfs->Create(nullptr, "/system/prc");
        ProcessRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        ProcessRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Process::~Process()
    {
    }
}
