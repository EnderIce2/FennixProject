#include <filesystem.h>
#include "../drivers/disk.h"

namespace FileSystem
{
    ReadFSFunction(FAT_Read)
    {
        return Size;
    }

    WriteFSFunction(FAT_Write)
    {
        return Size;
    }

    FileSystemOpeations fat = {
        .Name = "FAT",
        .Read = FAT_Read,
        .Write = FAT_Write};

// 0x28 or 0x29
#define FAT_MAGIC1 0x28
#define FAT_MAGIC2 0x29

    FAT::FatType FAT::GetFATType(BIOSParameterBlock *bpb)
    {
        if (bpb == nullptr || bpb == 0)
            return Unknown;
        if (bpb->BytesPerSector == 0)
            return Unknown;
        if (bpb->SectorsPerCluster == 0)
            return Unknown;

        if (bpb->JumpBoot[0] == 0xEB)
        {
            if (bpb->JumpBoot[2] != 0x90)
                return Unknown;
        }
        else if (bpb->JumpBoot[0] != 0xE9)
            return Unknown;

        // TODO: https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BPB20_OFS_0Ah
        if (bpb->Media != 0xF0 && bpb->Media < 0xF8)
            return Unknown;

        uint32_t Size;
        uint32_t Sectors;
        if (bpb->Sectors16 != 0)
            Size = bpb->Sectors16;
        else
            Size = bpb->Sectors32;

        if (bpb->Sectors16 != 0)
            Sectors = bpb->Sectors16;
        else
            Sectors = bpb->Sectors32;

        if (bpb->BytesPerSector != 512 && bpb->BytesPerSector != 1024 && bpb->BytesPerSector != 2048 && bpb->BytesPerSector != 4096)
            return Unknown;
        if (bpb->SectorsPerCluster * bpb->BytesPerSector > 32 * 1024)
            return Unknown;

        if (bpb->ReservedSectors == 0)
            return Unknown;

        if (bpb->NumberOfFATs < 2)
            return Unknown;

        uint32_t Clusters = (Sectors - (bpb->ReservedSectors + (bpb->NumberOfFATs * Size) + (((bpb->RootDirectoryEntries * 32) + (bpb->BytesPerSector - 1)) / bpb->BytesPerSector))) / bpb->SectorsPerCluster;

        if (Clusters < 4085)
        {
            debug("Detected FAT12");
            return FAT12;
        }
        else if (Clusters < 65525)
        {
            debug("Detected FAT16");
            return FAT16;
        }

        if (bpb->RootDirectoryEntries != 0)
        {
            debug("FAT12/16? (RootDirectoryEntries != 0)");
            return Unknown;
        }

        if (bpb->Sectors16 != 0)
        {
            debug("FAT12/16? (Sectors16 != 0)");
            return Unknown;
        }

        if (bpb->Sectors32 == 0)
        {
            debug("FAT12/16? (Sectors32 == 0)");
            return Unknown;
        }
        debug("OEM NAME: %c%c%c%c%c%c%c%c", bpb->OEM[0], bpb->OEM[1], bpb->OEM[2], bpb->OEM[3], bpb->OEM[4], bpb->OEM[5], bpb->OEM[6], bpb->OEM[7]);
        debug("JUMP BOOT: %c%c%c", bpb->JumpBoot[0], bpb->JumpBoot[1], bpb->JumpBoot[2]);
        return FAT32;
    }

    FAT::FAT(void *partition)
    {
        DiskManager::Partition::Part *Partition = (DiskManager::Partition::Part *)partition;

        void *buffer = RequestPage();
        Partition->Read(0, 1, buffer);
        switch (GetFATType((BIOSParameterBlock *)buffer))
        {
        case FAT12:
        // TODO: Implement reading and writing files
            debug("FAT12");
            mountfs->MountFileSystem(&fat, 0666, "stubfat12");
            break;
        case FAT16:
        // TODO: Implement reading and writing files
            debug("FAT16");
            mountfs->MountFileSystem(&fat, 0666, "stubfat16");
            break;
        case FAT32:
            debug("FAT32");
            // TODO: Implement reading and writing files
            mountfs->MountFileSystem(&fat, 0666, "stubfat32");
            break;
        default:
            break;
        }

        FreePage(buffer);
    }

    FAT::~FAT() { warn("Destroyed"); }
}