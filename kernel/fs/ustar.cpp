#include <filesystem.h>
#include <bootscreen.h>

namespace FileSystem
{
    ReadFSFunction(USTAR_Read)
    {
        if (!Size)
            Size = Node->Length;
        if (Offset > Node->Length)
            return 0;
        if (Offset + Size > Node->Length)
            Size = Node->Length - Offset;
        memcpy(Buffer, (uint8_t *)(Node->Address + Offset), Size);
        return Size;
    }

    FileSystemOpeations ustar = {
        .Name = "ustar",
        .Read = USTAR_Read,
    };

    USTAR::USTAR(uint64_t Address)
    {
        trace("INitializing USTAR with address %#llx", Address);
        if (memcmp(((FileHeader *)Address)->signature, "ustar", 5) != 0)
        {
            err("ustar signature invalid!");
            return;
        }
        debug("USTAR signature valid! Name:%s Signature:%s Mode:%c Size:%lu",
              ((FileHeader *)Address)->name,
              ((FileHeader *)Address)->signature,
              string2int(((FileHeader *)Address)->mode),
              ((FileHeader *)Address)->size);

        vfs->CreateRoot(&ustar, "/");

        uint64_t errorsallowed = 20;

        for (uint64_t i = 0;; i++)
        {
            FileHeader *header = (FileHeader *)Address;
            if (memcmp(((FileHeader *)Address)->signature, "ustar", 5) != 0)
                break;
            memmove(header->name, header->name + 1, strlen(header->name));
            if (header->name[strlen(header->name) - 1] == '/')
                header->name[strlen(header->name) - 1] = 0;
            uint64_t size = getsize(header->size);

            FileSystemNode *node = vfs->Create(nullptr, header->name);
            if (node == nullptr)
            {
                if (errorsallowed > 0)
                {
                    errorsallowed--;
                    goto NextFileAddress;
                }
                else
                {
                    err("Adding USTAR files failed because too many files were corrputed or invalid.");
                    break;
                }
            }
            else
            {
                BS->IncreaseProgres();
                trace("%s %dKB Type:%c", header->name, TO_KB(size), header->typeflag[0]);
                node->Mode = string2int(header->mode);
                node->Address = (Address + 512);
                node->Length = size;
                node->GroupIdentifier = getsize(header->gid);
                node->UserIdentifier = getsize(header->uid);
                node->IndexNode = i;

                switch (header->typeflag[0])
                {
                case REGULAR_FILE:
                    node->Flags = NodeFlags::FS_FILE;
                    break;
                case SYMLINK:
                    node->Flags = NodeFlags::FS_SYMLINK;
                    break;
                case DIRECTORY:
                    node->Flags = NodeFlags::FS_DIRECTORY;
                    break;
                case CHARDEV:
                    node->Flags = NodeFlags::FS_CHARDEVICE;
                    break;
                case BLOCKDEV:
                    node->Flags = NodeFlags::FS_BLOCKDEVICE;
                    break;
                default:
                    warn("Unknown type: %d", header->typeflag[0]);
                    break;
                }
            NextFileAddress:
                Address += ((size / 512) + 1) * 512;
                if (size % 512)
                    Address += 512;
            }
        }
    }

    USTAR::~USTAR() { warn("Destroyed"); }
}
