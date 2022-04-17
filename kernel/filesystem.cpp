#include <filesystem.h>
#include <string.h>
#include <cwalk.h>
#include <lock.h>
#include "kernel.h"
#include "drivers/disk.h"

NEWLOCK(VFSLock);

FileSystem::Virtual *vfs = nullptr;
FileSystem::Device *devfs = nullptr;
FileSystem::Mount *mountfs = nullptr;

namespace FileSystem
{
    FileSystemNode *FileSystemRoot = nullptr;

    Virtual::Virtual()
    {
        trace("Initializing virtual file system");
        FileSystemRoot = new FileSystemNode;
        FileSystemRoot->Flags = NodeFlags::MOUNTPOINT;
        FileSystemRoot->FileSystem = nullptr;
        FileSystemRoot->Parent = nullptr;
        strcpy(FileSystemRoot->Name, "root");
        cwk_path_set_style(CWK_STYLE_UNIX);
    }

    Virtual::~Virtual()
    {
        warn("Tried to uninitialize Virtual File System!");
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    FileSystemNode *DeviceRootNode;

    FileSystemNode *Device::AddFileSystem(struct FileSystemOpeations *Node, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        return nullptr;
    }

    Device::Device()
    {
        trace("Initializing device file system");
        DeviceRootNode->Mode = 0755;
    }

    Device::~Device()
    {
        warn("Tried to uninitialize Device File System!");
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    FileSystemNode *MountRootNode = nullptr;
    static uint64_t MountNodeIndexNodeCount = 0;

    FileSystemNode *Mount::MountFileSystem(FileSystemOpeations *Node, uint64_t Mode, string Name)
    {
        trace("Adding %s to mounted file systems", Name);
        return nullptr;
    }

    void Mount::DetectAndMountFS(void *drive)
    {
        foreach (auto partition in ((DiskManager::Partition::Drive *)drive)->Partitions)
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
    }

    Mount::~Mount()
    {
    }
}
