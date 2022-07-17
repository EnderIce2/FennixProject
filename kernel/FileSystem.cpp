#include <filesystem.h>
#include <string.h>
#include <cwalk.h>
#include <lock.h>
#include <printf.h>
#include <bootscreen.h>
#include "kernel.h"
#include "drivers/disk.h"

// show debug messages
// #define DEBUG_FILESYSTEM 1

#ifdef DEBUG_FILESYSTEM
#define vfsdbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define vfsdbg(m, ...)
#endif

NEWLOCK(VFSLock);

FileSystem::Virtual *vfs = nullptr;
FileSystem::Device *devfs = nullptr;
FileSystem::Mount *mountfs = nullptr;
FileSystem::Process *procfs = nullptr;

namespace FileSystem
{
    FileSystemNode *FileSystemRoot = nullptr;

    char *Virtual::GetPathFromNode(FileSystemNode *Node)
    {
        vfsdbg("GetPathFromNode( \"%s\" )", Node->Name);
        FileSystemNode *Parent = Node;
        Vector<char *> Path;
        size_t Size = 1;

        while (Parent != FileSystemRoot && Parent != nullptr)
        {
            foreach (auto var in FileSystemRoot->Children)
                if (var == Parent)
                    goto PathFromNodeContinue;
            Path.push_back(Parent->Name);
            Path.push_back((char *)"/");
            Parent = Parent->Parent;
        }

    PathFromNodeContinue:
        Path.reverse();

        foreach (auto var in Path)
            Size += strlen(var);

        char *FinalPath = new char[Size];

        foreach (auto var in Path)
            strcat(FinalPath, var);

        // for (size_t i = 0; i < Path.size(); i++)
        //     strcat(FinalPath, Path[i]);

        // for (size_t i = 0; i < Path.size(); i++)
        //     Size += strlen(Path[i]);
        vfsdbg("GetPathFromNode()->\"%s\"", FinalPath);
        return FinalPath;
    }

    FileSystemNode *Virtual::GetNodeFromPath(FileSystemNode *Parent, string Path)
    {
        vfsdbg("GetNodeFromPath( \"%s\" \"%s\" )", Parent->Name, Path);

        if (strcmp(Parent->Name, Path))
        {
            cwk_segment segment;
            if (!cwk_path_get_first_segment(Path, &segment))
            {
                err("Path doesn't have any segments.");
                return nullptr;
            }

            do
            {
                char *SegmentName = new char[segment.end - segment.begin + 1];
                memcpy(SegmentName, segment.begin, segment.end - segment.begin);
            GetNodeFromPathNextParent:
                foreach (auto var in Parent->Children)
                {
                    if (!strcmp(var->Name, SegmentName))
                    {
                        Parent = var;
                        goto GetNodeFromPathNextParent;
                    }
                }
            } while (cwk_path_get_next_segment(&segment));
            const char *basename;
            cwk_path_get_basename(Path, &basename, nullptr);
            if (!strcmp(basename, Parent->Name))
            {
                vfsdbg("GetNodeFromPath()->\"%s\"", Parent->Name);
                return Parent;
            }
        }
        else
        {
            vfsdbg("GetNodeFromPath()->\"%s\"", Parent->Name);
            return Parent;
        }

        vfsdbg("GetNodeFromPath()->\"%s\"", nullptr);
        return nullptr;
    }

    FileSystemNode *AddNewChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("AddNewChild( \"%s\" \"%s\" )", Parent->Name, Name);
        FileSystemNode *newNode = new FileSystemNode;
        newNode->Parent = Parent;
        strcpy(newNode->Name, Name);
        newNode->Operator = Parent->Operator;
        Parent->Children.push_back(newNode);
        vfsdbg("AddNewChild()->\"%s\"", newNode->Name);
        return newNode;
    }

    FileSystemNode *GetChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("GetChild( \"%s\" \"%s\" )", Parent->Name, Name);
        foreach (auto var in Parent->Children)
            if (strcmp(var->Name, Name) == 0)
            {
                vfsdbg("GetChild()->\"%s\"", var->Name);
                return var;
            }
        vfsdbg("GetChild()->nullptr");
        return nullptr;
    }

    FILESTATUS RemoveChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("RemoveChild( \"%s\" \"%s\" )", Parent->Name, Name);
        for (uint64_t i = 0; i < Parent->Children.size(); i++)
            if (strcmp(Parent->Children[i]->Name, Name) == 0)
            {
                Parent->Children.remove(i);
                vfsdbg("RemoveChild()->OK");
                return FILESTATUS::OK;
            }
        vfsdbg("RemoveChild()->NOT_FOUND");
        return FILESTATUS::NOT_FOUND;
    }

    char *Virtual::NormalizePath(FileSystemNode *Parent, string Path)
    {
        vfsdbg("NormalizePath( \"%s\" \"%s\" )", Parent->Name, Path);
        char *NormalizedPath = new char[strlen((char *)Path) + 1];
        char *RelativePath = nullptr;

        cwk_path_normalize(Path, NormalizedPath, strlen((char *)Path) + 1);

        if (cwk_path_is_relative(NormalizedPath))
        {
            size_t PathSize = cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, nullptr, 0);
            RelativePath = new char[PathSize + 1];
            cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, RelativePath, PathSize + 1);
        }
        else
        {
            RelativePath = new char[strlen(NormalizedPath) + 1];
            strcpy(RelativePath, NormalizedPath);
        }
        delete[] NormalizedPath;
        vfsdbg("NormalizePath()->\"%s\"", RelativePath);
        return RelativePath;
    }

    FILESTATUS FileExists(FileSystemNode *Parent, string Path)
    {
        // vfsdbg("FileExists( \"%s\" \"%s\" )", Parent->Name, Path);
        // TODO: implement FileExists()
        if (isempty((char *)Path))
            return FILESTATUS::INVALID_PATH;
        if (Parent == nullptr)
            Parent = FileSystemRoot;
        return FILESTATUS::NOT_FOUND;
    }

    FileSystemNode *Virtual::Create(FileSystemNode *Parent, string Path)
    {
        if (isempty((char *)Path))
            return nullptr;

        LOCK(VFSLock);
        vfsdbg("Virtual::Create( \"%s\" \"%s\" )", Parent->Name, Path);

        if (Parent == nullptr)
        {
            if (FileSystemRoot->Children.size() >= 1)
            {
                if (FileSystemRoot->Children[0] == nullptr)
                {
                    err("What?");
                }
                Parent = FileSystemRoot->Children[0]; // 0 - filesystem root
            }
            else
            {
                // TODO: check if here is a bug or something...
                string PathCopy;
                size_t length;
                PathCopy = (char *)Path;
                cwk_path_get_root(PathCopy, &length); // not working?
                foreach (auto var in FileSystemRoot->Children)
                    if (!strcmp(var->Name, PathCopy))
                    {
                        Parent = var;
                        break;
                    }
            }
        }

        char *CleanPath = NormalizePath(Parent, Path);
        bool parentcheck = false;

        if (FileExists(Parent, CleanPath) != FILESTATUS::NOT_FOUND)
        {
            err("File already exists.");
            goto CreatePathError;
        }

        cwk_segment segment;
        if (!cwk_path_get_first_segment(CleanPath, &segment))
        {
            err("Path doesn't have any segments.");
            goto CreatePathError;
        }

        do
        {
            // TODO: check if this is working properly.
            char *SegmentName = new char[segment.end - segment.begin + 1];
            memcpy(SegmentName, segment.begin, segment.end - segment.begin);
            if (!strcmp(SegmentName, Parent->Name) && !parentcheck)
            {
                parentcheck = true;
                delete[] SegmentName;
                continue;
            }
            if (GetChild(Parent, SegmentName) == nullptr)
                AddNewChild(Parent, SegmentName);
            Parent = GetChild(Parent, SegmentName);
            delete[] SegmentName;
        } while (cwk_path_get_next_segment(&segment));

        delete CleanPath;
        vfsdbg("Virtual::Create()->\"%s\"", Parent->Name);
        UNLOCK(VFSLock);
        return Parent;

    CreatePathError:
        vfsdbg("Virtual::Create return: nullptr");
        delete CleanPath;
        UNLOCK(VFSLock);
        return nullptr;
    }

    FileSystemNode *Virtual::CreateRoot(FileSystemOpeations *Operator, string RootName)
    {
        if (Operator == nullptr)
            return nullptr;
        vfsdbg("Setting root to %s", RootName);
        FileSystemNode *newNode = new FileSystemNode;
        strcpy(newNode->Name, RootName);
        newNode->Flags = NodeFlags::FS_DIRECTORY;
        newNode->Operator = Operator;
        FileSystemRoot->Children.push_back(newNode);
        return newNode;
    }

    FILE *Virtual::Mount(FileSystemOpeations *Operator, string Path)
    {
        if (Operator == nullptr)
            return nullptr;

        if (isempty((char *)Path))
            return nullptr;

        LOCK(VFSLock);
        vfsdbg("Mounting %s", Path);
        FILE *file = new FILE;
        cwk_path_get_basename(Path, &file->Name, 0);
        file->Status = FILESTATUS::OK;
        file->Node = Create(nullptr, Path);
        file->Node->Operator = Operator;
        file->Node->Flags = NodeFlags::FS_MOUNTPOINT;

        UNLOCK(VFSLock);
        return file;
    }

    FILESTATUS Virtual::Unmount(FILE *File)
    {
        if (File == nullptr)
            return FILESTATUS::INVALID_PARAMETER;

        LOCK(VFSLock);
        vfsdbg("Unmounting %s", File->Name);
        UNLOCK(VFSLock);
        return FILESTATUS::OK;
    }

    FILE *Virtual::Open(string Path, FileSystemNode *Parent)
    {
        vfsdbg("Opening %s", Path);

        if (Parent == nullptr)
        {
            if (FileSystemRoot->Children.size() >= 1)
                Parent = FileSystemRoot->Children[0]; // 0 - filesystem root
            else
            {
                // TODO: check if here is a bug or something...
                string PathCopy;
                size_t length;
                PathCopy = (char *)Path;
                cwk_path_get_root(PathCopy, &length); // not working?
                foreach (auto var in FileSystemRoot->Children)
                    if (!strcmp(var->Name, PathCopy))
                    {
                        Parent = var;
                        break;
                    }
            }
        }

        char *CleanPath = NormalizePath(Parent, Path);

        FILE *file = new FILE;
        FILESTATUS filestatus = FILESTATUS::OK;
        // TODO: NOT IMPLEMENTED YET
        // filestatus = FileExists(Parent, CleanPath);
        /* TODO: Check for other errors */

        if (filestatus != FILESTATUS::OK)
        {
            file->Status = filestatus;
            file->Node = nullptr;
        }
        else
        {
            file->Node = GetNodeFromPath(Parent, CleanPath);
            if (file->Node == nullptr)
                file->Status = FILESTATUS::NOT_FOUND;
            const char *basename;
            cwk_path_get_basename(CleanPath, &basename, nullptr);
            file->Name = basename;
            return file;
        }
        return file;
    }

    uint64_t Virtual::Read(FILE *File, uint64_t Offset, void *Buffer, uint64_t Size)
    {
        if (File == nullptr)
            return 0;

        File->Status = FILESTATUS::OK;

        if (File->Node == nullptr)
        {
            File->Status = FILESTATUS::INVALID_PARAMETER;
            return 0;
        }

        if (File->Node->Operator == nullptr)
        {
            File->Status = FILESTATUS::INVALID_PARAMETER;
            return 0;
        }
        vfsdbg("Reading %s out->%016x", File->Name, Buffer);
        return File->Node->Operator->Read(File->Node, Offset, Size, Buffer);
    }

    uint64_t Virtual::Write(FILE *File, uint64_t Offset, void *Buffer, uint64_t Size)
    {
        if (File == nullptr)
            return 0;

        File->Status = FILESTATUS::OK;

        if (File->Node == nullptr)
        {
            File->Status = FILESTATUS::INVALID_PARAMETER;
            return 0;
        }

        if (File->Node->Operator == nullptr)
        {
            File->Status = FILESTATUS::INVALID_PARAMETER;
            return 0;
        }
        vfsdbg("Writing %s out->%016x", File->Name, Buffer);
        return File->Node->Operator->Write(File->Node, Offset, Size, Buffer);
    }

    FILESTATUS Virtual::Close(FILE *File)
    {
        if (File == nullptr)
            return FILESTATUS::INVALID_HANDLE;
        vfsdbg("Closing %s", File->Name);
        delete File;

        return FILESTATUS::OK;
    }

    Virtual::Virtual()
    {
        trace("Initializing virtual file system");
        FileSystemRoot = new FileSystemNode;
        FileSystemRoot->Flags = NodeFlags::FS_MOUNTPOINT;
        FileSystemRoot->Operator = nullptr;
        FileSystemRoot->Parent = nullptr;
        strcpy(FileSystemRoot->Name, "root");
        cwk_path_set_style(CWK_STYLE_UNIX);
        BS->IncreaseProgres();
    }

    Virtual::~Virtual()
    {
        warn("Tried to uninitialize Virtual File System!");
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    FileSystemNode *DeviceRootNode;

    FileSystemNode *Device::AddFileSystem(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        FileSystemNode *newNode = vfs->Create(DeviceRootNode, Name);
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        return newNode;
    }

    Device::Device()
    {
        trace("Initializing device file system");
        DeviceRootNode = vfs->Create(nullptr, "/system/dev");
        DeviceRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        DeviceRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Device::~Device()
    {
        warn("Tried to uninitialize Device File System!");
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

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

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    FileSystemNode *ProcessRootNode = nullptr;

    Process::Process()
    {
        trace("Mounting file systems...");
        ProcessRootNode = vfs->Create(nullptr, "/system/proc");
        ProcessRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        ProcessRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Process::~Process()
    {
    }

}
