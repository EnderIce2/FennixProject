#pragma once
#include <types.h>
#include <vector.hpp>
#include "controllers/ahci.h"
#include "controllers/floppy.h"

extern Vector<AHCI::AHCIDriver *> ahci;
extern Floppy::FloppyDriver *floppy;

namespace DiskManager
{
    class Disk
    {
    public:
        Disk();
        ~Disk();
    };

    class Partition
    {
#define MBR_MAGIC0 0x55
#define MBR_MAGIC1 0xAA

// "EFI PART"
#define GPT_MAGIC 0x5452415020494645ULL

        enum PartitionStyle
        {
            Unknown,
            MBR,
            GPT
        };

        enum PartitionFlags
        {
            Present,
            Bootable,
            EFISystemPartition
        };

        struct MasterBootRecordPartition
        {
            uint8_t Flags;
            uint8_t CHSFirst[3];
            uint8_t Type;
            uint8_t CHSLast[3];
            uint32_t LBAFirst;
            uint32_t Sectors;
        } __attribute__((packed));

        struct MasterBootRecord
        {
            uint8_t Bootstrap[440];
            uint32_t UniqueID;
            uint16_t Reserved;
            MasterBootRecordPartition Partitions[4];
            uint8_t Signature[2];
        } __attribute__((packed));

        struct GUIDPartitionTablePartition
        {
            uint64_t TypeLow;
            uint64_t TypeHigh;
            uint64_t GUIDLow;
            uint64_t GUIDHigh;
            uint64_t StartLBA;
            uint64_t EndLBA;
            uint64_t Attributes;
            char Label[72];
        } __attribute__((packed));

        struct GUIDPartitionTable
        {
            uint64_t Signature;
            uint32_t Revision;
            uint32_t HdrSize;
            uint32_t HdrCRC32;
            uint32_t Reserved;
            uint64_t LBA;
            uint64_t ALTLBA;
            uint64_t FirstBlock;
            uint64_t LastBlock;
            uint64_t GUIDLow;
            uint64_t GUIDHigh;
            uint64_t PartLBA;
            uint32_t PartCount;
            uint32_t EntrySize;
            uint32_t PartCRC32;
        } __attribute__((packed));

        struct PartitionTable
        {
            MasterBootRecord MBR;
            GUIDPartitionTable GPT;
        };

    public:
        struct Part;

        class Drive
        {
        public:
            char Name[64] = "Unidentified Drive";
            uint8_t *Buffer = nullptr;
            PartitionTable Table;
            PartitionStyle Style = PartitionStyle::Unknown;
            Vector<Part *> Partitions;
            bool MechanicalDisk = false;
            uint64_t UniqueIdentifier = deadbeef;
        };

        struct Part
        {
            char Label[72] = "Unidentified Partition";
            uint64_t StartLBA = deadbeef;
            uint64_t EndLBA = deadbeef;
            uint64_t Sectors = deadbeef;
            uint64_t Flags = deadbeef;
            PartitionStyle Style = PartitionStyle::Unknown;
            Drive *Parent = nullptr;
            size_t Index = 0;
            AHCI::Port *Port = nullptr;

            uint64_t Read(uint64_t Offset, uint64_t Count, void *Buffer);
            uint64_t Write(uint64_t Offset, uint64_t Count, void *Buffer);
        };

        void AddPartition(AHCI::Port *Port);
        Partition();
        ~Partition();
    };
}

extern DiskManager::Disk *diskmgr;
extern DiskManager::Partition *partmgr;
extern Vector<DiskManager::Partition::Drive *> drives;
