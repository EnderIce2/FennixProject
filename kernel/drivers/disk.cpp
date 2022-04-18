#include "disk.h"
#include <filesystem.h>
#include <bootscreen.h>
#include <printf.h>
#include "serial.h"
#include "../pci.h"

Vector<AHCI::AHCIDriver *> ahci;
Floppy::FloppyDriver *floppy = nullptr;
DiskManager::Disk *diskmgr = nullptr;
DiskManager::Partition *partmgr = nullptr;

namespace DiskManager
{
    Disk::Disk()
    {
        trace("Detecting disks...");
        floppy = new Floppy::FloppyDriver();
        // TODO: detect for multiple ahci controllers.

        for (auto var : PCI::FindPCIDevice(0x01, 0x06, 0x01))
        {
            ahci.push_back(new AHCI::AHCIDriver(var));
            if (ahci.back()->PCIBaseAddress == 0)
                err("AHCI initialization error!");
        }
        BS->IncreaseProgres();
    }

    Disk::~Disk()
    {
        warn("Tried to uninitialize Disk driver!");
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    Vector<DiskManager::Partition::Drive *> drives;

    ReadFSFunction(StubDisk_Read)
    {
        warn("Reading not implemented yet");
        return Size;
    }
    WriteFSFunction(StubDisk_Write)
    {
        warn("Reading not implemented yet");
        return Size;
    }

    // TODO: implement reading mounted disks
    FileSystem::FileSystemOpeations stub_disk = {
        .Name = "StubDisk",
        .Read = StubDisk_Read,
        .Write = StubDisk_Write};

    void Partition::AddPartition(AHCI::Port *Port)
    {
        if (Port == nullptr)
        {
            err("Port is null!");
            return;
        }

        Drive *drive = new Drive;
        drives.push_back(drive);
        sprintf_(drive->Name, "dsk%lld", drives.size() - 1);
        // TODO: Implement disk type detection. Very useful in the future.
        drive->MechanicalDisk = true;

        Port->ReadWrite(0, 2, drive->Buffer, false);
        memcpy(&drive->Table, drive->Buffer, sizeof(PartitionTable));

        devfs->AddFileSystem(&stub_disk, 0666, drive->Name, FileSystem::NodeFlags::BLOCKDEVICE);

        if (drive->Table.GPT.Signature == GPT_MAGIC)
        {
            drive->Style = GPT;
            uint32_t Entries = 512 / drive->Table.GPT.EntrySize;
            uint32_t Sectors = drive->Table.GPT.PartCount / Entries;
            for (uint8_t Block = 0; Block < Sectors; Block++)
            {
                Port->ReadWrite(2 + Block, 1, drive->Buffer, false);
                for (uint8_t e = 0; e < Entries; e++)
                {
                    GUIDPartitionTablePartition GPTPartition = reinterpret_cast<GUIDPartitionTablePartition *>(drive->Buffer)[e];
                    if (GPTPartition.TypeLow || GPTPartition.TypeHigh)
                    {
                        Part *partition = new Part;
                        memcpy(partition->Label, GPTPartition.Label, sizeof(partition->Label));
                        partition->StartLBA = GPTPartition.StartLBA;
                        partition->EndLBA = GPTPartition.EndLBA;
                        partition->Sectors = partition->EndLBA - partition->StartLBA;
                        partition->Parent = drive;
                        partition->Port = Port;
                        partition->Flags = Present;
                        partition->Style = GPT;
                        if (GPTPartition.Attributes & 1)
                            partition->Flags |= EFISystemPartition;
                        partition->Index = drive->Partitions.size();
                        // TODO: Clean the Label (remove spaces, etc) and make it char* or string
                        trace("GPT partition \"%s\" found with %lld sectors", GPTPartition.Label, partition->Sectors);
                        drive->Partitions.push_back(partition);

                        char *PartitionName = (char *)kmalloc(sizeof(char));
                        sprintf_(PartitionName, "dsk%lldpart%lld", drives.size() - 1, partition->Index);
                        devfs->AddFileSystem(&stub_disk, 0666, PartitionName, FileSystem::NodeFlags::BLOCKDEVICE);
                    }
                }
            }
            trace("%d GPT partitions found.", drive->Partitions.size());
        }
        else if (drive->Table.MBR.Signature[0] == MBR_MAGIC0 && drive->Table.MBR.Signature[1] == MBR_MAGIC1)
        {
            drive->Style = MBR;
            for (size_t p = 0; p < 4; p++)
                if (drive->Table.MBR.Partitions[p].LBAFirst != 0)
                {
                    Part *partition = new Part;
                    partition->StartLBA = drive->Table.MBR.Partitions[p].LBAFirst;
                    partition->EndLBA = drive->Table.MBR.Partitions[p].LBAFirst + drive->Table.MBR.Partitions[p].Sectors;
                    partition->Sectors = drive->Table.MBR.Partitions[p].Sectors;
                    partition->Port = Port;
                    partition->Flags = Present;
                    partition->Style = MBR;
                    partition->Parent = drive;
                    partition->Index = drive->Partitions.size();
                    trace("Partition \"%#llx\" found with %lld sectors.", drive->Table.MBR.UniqueID, partition->Sectors);
                    drive->Partitions.push_back(partition);

                    char *PartitionName = (char *)kmalloc(sizeof(char));
                    sprintf_(PartitionName, "dsk%lldpart%lld", drives.size() - 1, partition->Index);
                    devfs->AddFileSystem(&stub_disk, 0666, PartitionName, FileSystem::NodeFlags::BLOCKDEVICE);
                }
            trace("%d MBR partitions found.", drive->Partitions.size());
        }
        else
            warn("No partition table found on port %d!", Port->PortNumber);

        mountfs->DetectAndMountFS(drive);
    }

    Partition::Partition()
    {
        trace("Mounting partitions...");
        for (auto var : ahci)
        {
            for (size_t i = 0; i < var->PortCount; i++)
                AddPartition(var->Ports[i]);
        }
        BS->IncreaseProgres();
    }

    Partition::~Partition()
    {
        warn("Tried to uninitialize Partition driver!");
    }
}
