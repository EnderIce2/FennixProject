#pragma once
#include <types.h>
#include <vector.hpp>

namespace FileSystem
{
#define FILENAME_LENGTH 256

    struct FileSystemNode;

    typedef uint64_t (*OperationMount)(const char *, unsigned long, const void *);
    typedef uint64_t (*OperationUmount)(int);
    typedef uint64_t (*OperationRead)(FileSystemNode *Node, uint64_t Offset, uint64_t Size, uint8_t *Buffer);
    typedef uint64_t (*OperationWrite)(FileSystemNode *Node, uint64_t Offset, uint64_t Size, uint8_t *Buffer);
    typedef void (*OperationOpen)(FileSystemNode *Node, uint8_t Mode, uint8_t Flags);
    typedef void (*OperationClose)(FileSystemNode *Node);
    typedef uint64_t (*OperationSync)(void);
    typedef void (*OperationCreate)(FileSystemNode *Node, char *Name, uint16_t NameLength);
    typedef void (*OperationMkdir)(FileSystemNode *Node, char *Name, uint16_t NameLength);

#define MountFSFunction(name) uint64_t name(const char *unknown0, unsigned long unknown1, const uint8_t *unknown2)
#define UMountFSFunction(name) uint64_t name(int unknown0)

#define ReadFSFunction(name) uint64_t name(FileSystem::FileSystemNode *Node, uint64_t Offset, uint64_t Size, uint8_t *Buffer)
#define WriteFSFunction(name) uint64_t name(FileSystem::FileSystemNode *Node, uint64_t Offset, uint64_t Size, uint8_t *Buffer)
#define OpenFSFunction(name) void name(FileSystem::FileSystemNode *Node, uint8_t Mode, uint8_t Flags)
#define CloseFSFunction(name) void name(FileSystem::FileSystemNode *Node)
#define SyncFSFunction(name) uint64_t name(void)
#define CreateFSFunction(name) void name(FileSystem::FileSystemNode *Node, char *Name, uint16_t NameLength)
#define MkdirFSFunction(name) void name(FileSystem::FileSystemNode *Node, char *Name, uint16_t NameLength)

    enum FILESTATUS
    {
        OK = 0,
        NOT_FOUND = 1,
        ACCESS_DENIED = 2,
        INVALID_NAME = 3,
        INVALID_PARAMETER = 4,
        INVALID_HANDLE = 5,
        INVALID_PATH = 6,
        INVALID_FILE = 7,
        INVALID_DEVICE = 8,
        NOT_EMPTY = 9,
        NOT_SUPPORTED = 10,
        INVALID_DRIVE = 11,
        VOLUME_IN_USE = 12,
        TIMEOUT = 13,
        NO_MORE_FILES = 14,
        END_OF_FILE = 15,
        FILE_EXISTS = 16,
        PIPE_BUSY = 17,
        PIPE_DISCONNECTED = 18,
        MORE_DATA = 19,
        NO_DATA = 20,
        PIPE_NOT_CONNECTED = 21,
        MORE_ENTRIES = 22,
        DIRECTORY_NOT_EMPTY = 23,
        NOT_A_DIRECTORY = 24,
        FILE_IS_A_DIRECTORY = 25,
        DIRECTORY_NOT_ROOT = 26,
        DIRECTORY_NOT_EMPTY_2 = 27,
        END_OF_MEDIA = 28,
        NO_MEDIA = 29,
        UNRECOGNIZED_MEDIA = 30,
        SECTOR_NOT_FOUND = 31
    };

    enum NodeFlags
    {
        FS_ERROR = 0x0,
        FS_FILE = 0x01,
        FS_DIRECTORY = 0x02,
        FS_CHARDEVICE = 0x03,
        FS_BLOCKDEVICE = 0x04,
        FS_PIPE = 0x05,
        FS_SYMLINK = 0x06,
        FS_MOUNTPOINT = 0x08
    };

    struct FileSystemOpeations
    {
        char Name[FILENAME_LENGTH];
        OperationMount Mount = nullptr;
        OperationUmount Umount = nullptr;
        OperationRead Read = nullptr;
        OperationWrite Write = nullptr;
        OperationOpen Open = nullptr;
        OperationClose Close = nullptr;
        OperationCreate Create = nullptr;
        OperationMkdir MakeDirectory = nullptr;
    };

    struct FileSystemNode
    {
        char Name[FILENAME_LENGTH];
        uint64_t IndexNode = 0;
        uint64_t Mask = 0;
        uint64_t Mode = 0;
        uint64_t Flags = NodeFlags::FS_ERROR;
        uint64_t UserIdentifier = 0, GroupIdentifier = 0;
        uint64_t Address = 0;
        uint64_t Length = 0;
        FileSystemNode *Parent = nullptr;
        FileSystemOpeations *Operator = nullptr;
        /* For root node:
        0 - root "/"
        1 - tba
        ...
        */
        Vector<FileSystemNode *> Children;
    };

    struct FILE
    {
        string Name;
        FILESTATUS Status;
        FileSystemNode *Node;
    };

    /* Manage / etc.. */
    class Virtual
    {
    public:
        FILE *ConvertNodeToFILE(FileSystemNode *Node)
        {
            FILE *File = new FILE();
            File->Name = Node->Name;
            File->Status = FILESTATUS::OK;
            File->Node = Node;
            return File;
        }
        char *GetPathFromNode(FileSystemNode *Node);
        FileSystemNode *GetNodeFromPath(FileSystemNode *Parent, string Path);
        char *NormalizePath(FileSystemNode *Parent, string Path);

        FILE *Mount(FileSystemOpeations *Operator, string Path);
        FILESTATUS Unmount(FILE *File);
        FILE *Open(string Path, FileSystemNode *Parent = nullptr);
        uint64_t Read(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size);
        uint64_t Write(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size);
        FILESTATUS Close(FILE *File);
        FileSystemNode *CreateRoot(FileSystemOpeations *Operator, string RootName);
        FileSystemNode *Create(FileSystemNode *Parent, string Path);

        Virtual();
        ~Virtual();
    };

    /* Manage /system/dev */
    class Device
    {
    public:
        FileSystemNode *AddFileSystem(FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags);
        Device();
        ~Device();
    };

    /* Manage /system/mnt */
    class Mount
    {
    public:
        FileSystemNode *MountFileSystem(FileSystemOpeations *Operator, uint64_t Mode, string Name);
        void DetectAndMountFS(void *drive);
        Mount();
        ~Mount();
    };

    /* Manage /system/prc */
    class Process
    {
    public:
        Process();
        ~Process();
    };

    /* Manage /system/drv */
    class Driver
    {
    public:
        FileSystemNode *AddDriver(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags);
        Driver();
        ~Driver();
    };

    /* Manage /system/net */
    class Network
    {
    public:
        FileSystemNode *AddNetworkCard(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags);
        Network();
        ~Network();
    };

    class USTAR
    {
        enum FileType
        {
            REGULAR_FILE = '0',
            HARDLINK = '1',
            SYMLINK = '2',
            CHARDEV = '3',
            BLOCKDEV = '4',
            DIRECTORY = '5',
            FIFO = '6'
        };

        struct FileHeader
        {
            char name[100];
            char mode[8];
            char uid[8];
            char gid[8];
            char size[12];
            char mtime[12];
            char chksum[8];
            char typeflag[1];
            char link[100];
            char signature[6];
            char version[2];
            char owner[32];
            char group[32];
            char dev_maj[8];
            char dev_min[8];
            char prefix[155];
            char pad[12];
        };

    private:
        uint32_t getsize(string s)
        {
            uint64_t ret = 0;
            while (*s)
            {
                ret *= 8;
                ret += *s - '0';
                s++;
            }
            return ret;
        }

        int string2int(string str)
        {
            int res = 0;
            for (int i = 0; str[i] != '\0'; ++i)
                res = res * 10 + str[i] - '0';
            return res;
        }

    public:
        USTAR(uint64_t Address);
        ~USTAR();
    };

    class Initrd
    {
    public:
        struct InitrdHeader
        {
            uint32_t nfiles;
        };

        struct InitrdFileHeader
        {
            uint8_t magic;
            char name[64];
            uint32_t offset;
            uint32_t length;
        };

        Initrd(uint64_t Address);
        ~Initrd();
    };

    class Serial
    {
    public:
        Serial();
        ~Serial();
    };

    class Random
    {
    public:
        Random();
        ~Random();
    };

    class Null
    {
    public:
        Null();
        ~Null();
    };

    class Zero
    {
    public:
        Zero();
        ~Zero();
    };

    class FB
    {
    public:
        FB();
        ~FB();
    };

    class EXT2
    {
    public:
        struct SuperBlock
        {
            uint32_t Inodes;
            uint32_t Blocks;
            uint32_t ReservedBlocks;
            uint32_t FreeBlock;
            uint32_t FreeInodes;
            uint32_t FirstDataBlock;
            uint32_t LogBlockSize;
            uint32_t LogFragSize;
            uint32_t BlocksPerGroup;
            uint32_t FragsPerGroup;
            uint32_t InodesPerGroup;
            uint32_t LastMountTime;
            uint32_t LastWrittenTime;
            uint16_t MountedTimes;
            uint16_t MaximumMountedTimes;
            uint16_t Magic;
            uint16_t State;
            uint16_t Errors;
            uint16_t MinorRevLevel;
            uint32_t LastCheck;
            uint32_t CheckInternval;
            uint32_t SystemID;
            uint32_t RevLevel;
            uint16_t ReservedBlocksUserID;
            uint16_t ReservedBlocksGroupID;

            uint32_t FirstInode;
            uint16_t InodeSize;
            uint16_t BlockGroups;
            uint32_t FeatureCompatibility;
            uint32_t FeatureIncompatibility;
            uint32_t FeatureRoCompatibility;
            uint8_t UUID[16];
            char VolumeName[16];
            char LastMounted[64];
            uint32_t BitmapAlogrithm;

            uint8_t PreallocatedBlocks;
            uint8_t PreallocatedDirectoryBlocks;

            uint16_t Padding;
            uint8_t JournalUUID[16];
            uint32_t JournalInum;
            uint32_t JournalDev;
            uint32_t LastOrphan;
            uint32_t HashSeed[4];
            uint8_t DefHashVersion;
            uint8_t ReservedCharPad;
            uint16_t ReservedWordPad;
            uint32_t DefaultMountOptions;
            uint32_t FirstMetaBg;
            uint32_t Reserved[190];
        };

        EXT2(void *partition);
        ~EXT2();
    };

    class FAT
    {
    public:
        enum FatType
        {
            Unknown,
            FAT12,
            FAT16,
            FAT32
        };

        /* https://wiki.osdev.org/FAT */
        struct BIOSParameterBlock
        {
            /** @brief The first three bytes EB 3C 90 disassemble to JMP SHORT 3C NOP. (The 3C value may be different.) The reason for this is to jump over the disk format information (the BPB and EBPB). Since the first sector of the disk is loaded into ram at location 0x0000:0x7c00 and executed, without this jump, the processor would attempt to execute data that isn't code. Even for non-bootable volumes, code matching this pattern (or using the E9 jump opcode) is required to be present by both Windows and OS X. To fulfil this requirement, an infinite loop can be placed here with the bytes EB FE 90. */
            uint8_t JumpBoot[3];
            /** @brief OEM identifier. The first 8 Bytes (3 - 10) is the version of DOS being used. The next eight Bytes 29 3A 63 7E 2D 49 48 and 43 read out the name of the version. The official FAT Specification from Microsoft says that this field is really meaningless and is ignored by MS FAT Drivers, however it does recommend the value "MSWIN4.1" as some 3rd party drivers supposedly check it and expect it to have that value. Older versions of dos also report MSDOS5.1, linux-formatted floppy will likely to carry "mkdosfs" here, and FreeDOS formatted disks have been observed to have "FRDOS5.1" here. If the string is less than 8 bytes, it is padded with spaces. */
            uint8_t OEM[8];
            /** @brief The number of Bytes per sector (remember, all numbers are in the little-endian format). */
            uint16_t BytesPerSector;
            /** @brief Number of sectors per cluster. */
            uint8_t SectorsPerCluster;
            /** @brief Number of reserved sectors. The boot record sectors are included in this value. */
            uint16_t ReservedSectors;
            /** @brief Number of File Allocation Tables (FAT's) on the storage media. Often this value is 2. */
            uint8_t NumberOfFATs;
            /** @brief Number of root directory entries (must be set so that the root directory occupies entire sectors). */
            uint16_t RootDirectoryEntries;
            /** @brief The total sectors in the logical volume. If this value is 0, it means there are more than 65535 sectors in the volume, and the actual count is stored in the Large Sector Count entry at 0x20. */
            uint16_t Sectors16;
            /** @brief This Byte indicates the media descriptor type. */
            uint8_t Media;
            /** @brief Number of sectors per FAT. FAT12/FAT16 only. */
            uint16_t SectorsPerFAT;
            /** @brief Number of sectors per track. */
            uint16_t SectorsPerTrack;
            /** @brief Number of heads or sides on the storage media. */
            uint16_t NumberOfHeads;
            /** @brief Number of hidden sectors. (i.e. the LBA of the beginning of the partition). */
            uint32_t HiddenSectors;
            /** @brief Large sector count. This field is set if there are more than 65535 sectors in the volume, resulting in a value which does not fit in the Number of Sectors entry at 0x13. */
            uint32_t Sectors32;
        } __attribute__((__packed__));

        FatType GetFATType(BIOSParameterBlock *bpb);
        FAT(void *partition);
        ~FAT();
    };
}

extern FileSystem::Virtual *vfs;
extern FileSystem::Device *devfs;
extern FileSystem::Mount *mountfs;
extern FileSystem::Process *procfs;
extern FileSystem::Driver *drvfs;
extern FileSystem::Network *netfs;
