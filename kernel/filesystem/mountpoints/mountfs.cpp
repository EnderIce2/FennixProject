#include <filesystem.h>

#include <bootscreen.h>
#include <printf.h>

#include "../drivers/disk.h"

namespace FileSystem
{
    FileSystemNode *MountRootNode = nullptr;
    static uint64_t MountNodeIndexNodeCount = 0;

    FileSystemNode *Mount::MountFileSystem(FileSystemOpeations *Operator, uint64_t Mode, string Name)
    {
        if (isempty((char *)Name))
        {
            warn("Tried to mount file system with empty name!");
            sprintf_((char *)Name, "mount_%lu", MountNodeIndexNodeCount);
        }
        trace("Adding %s to mounted file systems", Name);
        FileSystemNode *newNode = vfs->Create(MountRootNode, Name);
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = NodeFlags::FS_MOUNTPOINT;
        return newNode;
    }

    void Mount::DetectAndMountFS(void *drive)
    {
        foreach (auto partition in((DiskManager::Partition::Drive *)drive)->Partitions)
        {
            debug("Mounting File Systems for Partition %d...", partition->Index);
            new EXT2(partition);
            new FAT(partition);
            /* ... */
        }
    }

    Mount::Mount()
    {
        trace("Mounting file systems...");
        MountRootNode = vfs->Create(nullptr, "/system/mnt");
        MountRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        MountRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Mount::~Mount()
    {
    }
}
