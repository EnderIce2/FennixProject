#include <filesystem.h>

#include <bootscreen.h>
#include <smartptr.hpp>
#include <string.h>
#include <printf.h>
#include <cwalk.h>
#include <lock.h>
#include <sys.h>

#include "../drivers/disk.h"
#include "../kernel.h"

NEWLOCK(VFSLock);

FileSystem::Virtual *vfs = nullptr;
FileSystem::Device *devfs = nullptr;
FileSystem::Mount *mountfs = nullptr;
FileSystem::Process *procfs = nullptr;
FileSystem::Driver *drvfs = nullptr;
FileSystem::Network *netfs = nullptr;
FileSystem::SysInfo *sysfs = nullptr;

namespace FileSystem
{
    char *Virtual::GetPathFromNode(FileSystemNode *Node)
    {
        vfsdbg("GetPathFromNode( Node: \"%s\" )", Node->Name);
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
        vfsdbg("GetNodeFromPath( Parent: \"%s\" Path: \"%s\" )", Parent->Name, Path);

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
                delete[] SegmentName;
            } while (cwk_path_get_next_segment(&segment));
            const char *basename;
            cwk_path_get_basename(Path, &basename, nullptr);
            if (!strcmp(basename, Parent->Name))
            {
                vfsdbg("GetNodeFromPath()->\"%s\"", Parent->Name);
                return Parent;
            }

            vfsdbg("GetNodeFromPath()->\"%s\"", nullptr);
            return nullptr;
        }
        else
        {
            vfsdbg("GetNodeFromPath()->\"%s\"", Parent->Name);
            return Parent;
        }
    }

    FileSystemNode *AddNewChild(FileSystemNode *Parent, string Name)
    {
        vfsdbg("AddNewChild( Parent: \"%s\" Name: \"%s\" )", Parent->Name, Name);
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
        vfsdbg("GetChild( Parent: \"%s\" Name: \"%s\" )", Parent->Name, Name);
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
        vfsdbg("RemoveChild( Parent: \"%s\" Name: \"%s\" )", Parent->Name, Name);
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
        vfsdbg("NormalizePath( Parent: \"%s\" Path: \"%s\" )", Parent->Name, Path);
        char *NormalizedPath = new char[strlen((char *)Path) + 1];
        char *RelativePath = nullptr;

        cwk_path_normalize(Path, NormalizedPath, strlen((char *)Path) + 1);

        if (cwk_path_is_relative(NormalizedPath))
        {
            char *ParentPath = GetPathFromNode(Parent);
            size_t PathSize = cwk_path_get_absolute(ParentPath, NormalizedPath, nullptr, 0);
            RelativePath = new char[PathSize + 1];
            cwk_path_get_absolute(ParentPath, NormalizedPath, RelativePath, PathSize + 1);
            delete[] ParentPath;
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

    FILESTATUS Virtual::FileExists(FileSystemNode *Parent, string Path)
    {
        vfsdbg("FileExists( Parent: \"%s\" Path: \"%s\" )", Parent->Name, Path);
        if (isempty((char *)Path))
            return FILESTATUS::INVALID_PATH;
        if (Parent == nullptr)
            Parent = FileSystemRoot;

        char *NormalizedPath = NormalizePath(Parent, Path);
        FileSystemNode *Node = GetNodeFromPath(Parent, NormalizedPath);

        if (Node == nullptr)
        {
            vfsdbg("FileExists()->NOT_FOUND");
            return FILESTATUS::NOT_FOUND;
        }
        else
        {
            vfsdbg("FileExists()->OK");
            return FILESTATUS::OK;
        }
    }

    FileSystemNode *Virtual::Create(FileSystemNode *Parent, string Path)
    {
        SMART_LOCK(VFSLock);

        if (isempty((char *)Path))
            return nullptr;

        vfsdbg("Virtual::Create( Parent: \"%s\" Path: \"%s\" )", Parent->Name, Path);

        FileSystemNode *CurrentParent = nullptr;

        if (Parent == nullptr)
        {
            if (FileSystemRoot->Children.size() >= 1)
            {
                if (FileSystemRoot->Children[0] == nullptr)
                    panic("Root node is null!", true);

                CurrentParent = FileSystemRoot->Children[0]; // 0 - filesystem root
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
                        CurrentParent = var;
                        break;
                    }
            }
        }
        else
            CurrentParent = Parent;

        char *CleanPath = NormalizePath(CurrentParent, Path);

        if (FileExists(CurrentParent, CleanPath) != FILESTATUS::NOT_FOUND)
        {
            err("File %s already exists.", CleanPath);
            goto CreatePathError;
        }

        cwk_segment segment;
        if (!cwk_path_get_first_segment(CleanPath, &segment))
        {
            err("Path doesn't have any segments.");
            goto CreatePathError;
        }

        warn("Virtual::Create( ) is not working properly.");
        do
        {
            char *SegmentName = new char[segment.end - segment.begin + 1];
            memcpy(SegmentName, segment.begin, segment.end - segment.begin);

            if (GetChild(CurrentParent, SegmentName) == nullptr)
                CurrentParent = AddNewChild(CurrentParent, SegmentName);
            else
                CurrentParent = GetChild(CurrentParent, SegmentName);

            delete[] SegmentName;
        } while (cwk_path_get_next_segment(&segment));

        delete CleanPath;
        vfsdbg("Virtual::Create()->\"%s\"", CurrentParent->Name);
        return CurrentParent;

    CreatePathError:
        vfsdbg("Virtual::Create()->nullptr");
        delete CleanPath;
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
        SMART_LOCK(VFSLock);

        if (Operator == nullptr)
            return nullptr;

        if (isempty((char *)Path))
            return nullptr;

        vfsdbg("Mounting %s", Path);
        FILE *file = new FILE;
        cwk_path_get_basename(Path, &file->Name, 0);
        file->Status = FILESTATUS::OK;
        file->Node = Create(nullptr, Path);
        file->Node->Operator = Operator;
        file->Node->Flags = NodeFlags::FS_MOUNTPOINT;
        return file;
    }

    FILESTATUS Virtual::Unmount(FILE *File)
    {
        SMART_LOCK(VFSLock);
        if (File == nullptr)
            return FILESTATUS::INVALID_PARAMETER;
        vfsdbg("Unmounting %s", File->Name);
        return FILESTATUS::OK;
    }

    FILE *Virtual::Open(string Path, FileSystemNode *Parent)
    {
        SMART_LOCK(VFSLock);
        vfsdbg("Opening %s with parent %s", Path, Parent->Name);

        if (strcmp(Path, ".") == 0)
        {
            FILE *file = new FILE;
            FILESTATUS filestatus = FILESTATUS::OK;
            file->Node = Parent;
            if (file->Node == nullptr)
                file->Status = FILESTATUS::NOT_FOUND;
            const char *basename;
            cwk_path_get_basename(GetPathFromNode(Parent), &basename, nullptr);
            file->Name = basename;
            return file;
        }

        if (strcmp(Path, "..") == 0)
        {
            if (Parent->Parent != nullptr)
                Parent = Parent->Parent;

            FILE *file = new FILE;
            FILESTATUS filestatus = FILESTATUS::OK;
            file->Node = Parent;
            if (file->Node == nullptr)
                file->Status = FILESTATUS::NOT_FOUND;
            const char *basename;
            cwk_path_get_basename(GetPathFromNode(Parent), &basename, nullptr);
            file->Name = basename;
            return file;
        }

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
        filestatus = FileExists(Parent, CleanPath);
        /* TODO: Check for other errors */

        if (filestatus != FILESTATUS::OK)
        {
            foreach (auto var in FileSystemRoot->Children)
                if (!strcmp(var->Name, CleanPath))
                {
                    file->Node = var;
                    if (file->Node == nullptr)
                        goto OpenNodeFail;
                    const char *basename;
                    cwk_path_get_basename(GetPathFromNode(var), &basename, nullptr);
                    file->Name = basename;
                    goto OpenNodeExit;
                }

            file->Node = GetNodeFromPath(FileSystemRoot->Children[0], CleanPath);
            if (file->Node != nullptr)
            {
                const char *basename;
                cwk_path_get_basename(GetPathFromNode(file->Node), &basename, nullptr);
                file->Name = basename;
                goto OpenNodeExit;
            }

        OpenNodeFail:
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
    OpenNodeExit:
        return file;
    }

    uint64_t Virtual::Read(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
    {
        SMART_LOCK(VFSLock);
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

    uint64_t Virtual::Write(FILE *File, uint64_t Offset, uint8_t *Buffer, uint64_t Size)
    {
        SMART_LOCK(VFSLock);
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
        SMART_LOCK(VFSLock);
        if (File == nullptr)
            return FILESTATUS::INVALID_HANDLE;
        vfsdbg("Closing %s", File->Name);
        delete File;
        return FILESTATUS::OK;
    }

    Virtual::Virtual()
    {
        trace("Initializing virtual file system...");
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
}
