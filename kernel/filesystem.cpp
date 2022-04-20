#include <filesystem.h>
#include <string.h>
#include <cwalk.h>
#include <lock.h>
#include <bootscreen.h>
#include "kernel.h"
#include "drivers/disk.h"

// show debug messages
#define DEBUG_FILESYSTEM 1

#ifdef DEBUG_FILESYSTEM
#define vfsdbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define vfsdbg(m, ...)
#endif

NEWLOCK(VFSLock);

FileSystem::Virtual *vfs = nullptr;
FileSystem::Device *devfs = nullptr;
FileSystem::Mount *mountfs = nullptr;

namespace FileSystem
{
    FileSystemNode *FileSystemRoot = nullptr;

    FILESTATUS FileExists(FileSystemNode *Parent, string Path)
    {
        if (isempty((char *)Path))
            return FILESTATUS::INVALID_PATH;
        fixme("FileExists: %s", Path);
        if (Parent == nullptr)
            Parent = FileSystemRoot;
        return FILESTATUS::NOT_FOUND;
    }

    char *GetPathFromNode(FileSystemNode *Node)
    {
        vfsdbg("GetPathFromNode: %s", Node->Name);
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
        vfsdbg("FinalPath: %s", FinalPath);
        return FinalPath;
    }

    FileSystemNode *AddNewChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("AddNewChild: %s", Name);
        FileSystemNode *newNode = new FileSystemNode;
        newNode->Parent = Parent;
        strcpy(newNode->Name, Name);
        newNode->Operator = Parent->Operator;
        Parent->Children.push_back(newNode);
        vfsdbg("AddNewChild return: %s", newNode->Name);
        return newNode;
    }

    FileSystemNode *GetChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("GetChild: %s", Name);
        foreach (auto var in Parent->Children)
            if (strcmp(var->Name, Name) == 0)
            {
                vfsdbg("GetChild return: %s", var->Name);
                return var;
            }
        vfsdbg("GetChild return: nullptr");
        return nullptr;
    }

    FILESTATUS RemoveChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("RemoveChild: %s", Name);
        for (uint64_t i = 0; i < Parent->Children.size(); i++)
            if (strcmp(Parent->Children[i]->Name, Name) == 0)
            {
                Parent->Children.remove(i);
                vfsdbg("RemoveChild STATUS OK");
                return FILESTATUS::OK;
            }
        vfsdbg("RemoveChild STATUS NOT_FOUND");
        return FILESTATUS::NOT_FOUND;
    }

    FileSystemNode *Virtual::Create(FileSystemNode *Parent, string Path)
    {
        if (isempty((char *)Path))
            return nullptr;

        LOCK(VFSLock);
        vfsdbg("Virtual::Create: %s", Path);

        if (Parent == nullptr)
        {
            if (FileSystemRoot->Children.size() == 1)
                Parent = FileSystemRoot->Children[0];
            else
            {
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

        char *NormalizedPath = new char[strlen((char *)Path) + 1];
        char *RelativePath = nullptr;

        cwk_path_normalize(Path, NormalizedPath, strlen((char *)Path) + 1);

        if (cwk_path_is_relative(NormalizedPath))
        {
            size_t PathSize = cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, nullptr, 0);
            RelativePath = new char[PathSize];
            cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, RelativePath, PathSize);
        }
        else
        {
            RelativePath = new char[strlen(NormalizedPath) + 1];
            strcpy(RelativePath, NormalizedPath);
        }

        if (FileExists(Parent, Path) != FILESTATUS::NOT_FOUND)
        {
            err("File already exists.");
            goto CreatePathError;
        }

        cwk_segment segment;
        if (!cwk_path_get_first_segment(NormalizedPath, &segment))
        {
            err("Path doesn't have any segments.");
            goto CreatePathError;
        }

        do
        {
            if (GetChild(Parent, segment.begin) == nullptr)
                AddNewChild(Parent, segment.begin);
            Parent = GetChild(Parent, segment.begin);
        } while (cwk_path_get_next_segment(&segment));

        vfsdbg("Virtual::Create return: RelativePath: %s NormalizedPath: %s (Node: %s)", RelativePath, NormalizedPath, Parent->Name);
        delete NormalizedPath;
        delete RelativePath;
        UNLOCK(VFSLock);
        return Parent;

    CreatePathError:
        delete NormalizedPath;
        UNLOCK(VFSLock);
        return nullptr;
    }

    FILESTATUS Virtual::SetRoot(FileSystemOpeations *Operator, string RootName)
    {
        if (Operator == nullptr)
            return FILESTATUS::INVALID_PARAMETER;
        vfsdbg("Setting root to %s", RootName);
        FileSystemNode *newNode = new FileSystemNode;
        strcpy(newNode->Name, RootName);
        newNode->Flags = NodeFlags::FS_DIRECTORY;
        newNode->Operator = Operator;
        // good idea to support multiple roots?
        FileSystemRoot->Children.push_back(newNode);
        return FILESTATUS::OK;
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
            if (FileSystemRoot->Children.size() == 1)
                Parent = FileSystemRoot->Children[0];
            else
            {
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

        char *NormalizedPath = new char(strlen((char *)Path) + 1);
        char *RelativePath = nullptr;
        cwk_path_normalize(Path, NormalizedPath, strlen((char *)Path) + 1);

        if (cwk_path_is_relative(NormalizedPath))
        {
            size_t PathSize = cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, nullptr, 0);
            RelativePath = new char[PathSize];
            cwk_path_get_absolute(GetPathFromNode(Parent), NormalizedPath, RelativePath, PathSize);
        }
        else
        {
            RelativePath = new char[strlen(NormalizedPath) + 1];
            strcpy(RelativePath, NormalizedPath);
        }

        FILE *file = new FILE;
        cwk_path_get_basename(Path, &file->Name, 0);
        FILESTATUS filestatus = FILESTATUS::OK;
        // TODO: NOT IMPLEMENTED YET
        // filestatus = FileExists(Parent, NormalizedPath);
        /* TODO: Check for other errors */

        if (filestatus != FILESTATUS::OK)
        {
            file->Status = filestatus;
            file->Node = nullptr;
        }
        else
        {
            file->Status = FILESTATUS::OK;
            if (strcmp(Parent->Name, RelativePath))
            {
                foreach (auto var in Parent->Children)
                    if (!strcmp(var->Name, RelativePath))
                    {
                        file->Node = var;
                        return file;
                    }
            }
            else
            {
                file->Node = Parent;
                return file;
            }
        }
        file->Status = FILESTATUS::NOT_FOUND;
        return file;
    }

    uint64_t Virtual::Read(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
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
        vfsdbg("Reading %s", File->Name);
        return File->Node->Operator->Read(File->Node, Offset, Size, Buffer);
    }

    uint64_t Virtual::Write(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
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
        vfsdbg("Writing %s", File->Name);
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
        DeviceRootNode = vfs->Create(nullptr, "/dev");
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
        trace("Adding %s to mounted file systems", Name);
        FileSystemNode *newNode = vfs->Create(MountRootNode, Name);
        newNode->Mode = Mode;
        newNode->Operator = Operator;
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
        MountRootNode = vfs->Create(nullptr, "/mnt");
        BS->IncreaseProgres();
    }

    Mount::~Mount()
    {
    }
}
