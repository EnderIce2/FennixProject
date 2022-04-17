#include <filesystem.h>
#include "../drivers/disk.h"
#include "../drivers/serial.h"

namespace FileSystem
{
#define EXT2_MAGIC 0xEF53
    EXT2::SuperBlock sb;

    ReadFSFunction(EXT2_Read)
    {
        return Size;
    }

    WriteFSFunction(EXT2_Write)
    {
        return Size;
    }

    FileSystemOpeations ext2 = {
        .Name = "EXT2",
        .Read = EXT2_Read,
        .Write = EXT2_Write};

    EXT2::EXT2(void *partition)
    {
        DiskManager::Partition::Part *Partition = (DiskManager::Partition::Part *)partition;

        // Partition->Read(1024, 2, (unsigned char *)&sb);

        void *buffer = RequestPage();
        Partition->Read(2, 2, buffer);
        memcpy(&sb, buffer, sizeof(SuperBlock));
        FreePage(buffer);

        if (sb.Magic == EXT2_MAGIC)
        {
            debug("EXT2 Name: \"%s\" Last Mounted In: \"%s\"", sb.VolumeName, sb.LastMounted);
            // TODO: Implement reading and writing files
            mountfs->MountFileSystem(&ext2, 0666, "stubext2");
        }
    }

    EXT2::~EXT2() { warn("Destroyed"); }
}