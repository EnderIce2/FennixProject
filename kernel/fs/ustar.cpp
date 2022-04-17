#include <filesystem.h>

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

        for (uint64_t i = 0;; i++)
        {
            FileHeader *header = (FileHeader *)Address;
            if (memcmp(((FileHeader *)Address)->signature, "ustar", 5) != 0)
                break;
            memmove(header->name, header->name + 1, strlen(header->name));
            if (header->name[strlen(header->name) - 1] == '/')
                header->name[strlen(header->name) - 1] = 0;
            uint64_t size = getsize(header->size);
            trace("%s %dKB", header->name, TO_KB(size));

            FileSystemNode *node = new FileSystemNode;
            node->Mode = string2int(header->mode);
            node->Address = (Address + 512);
            node->Length = size;
            node->GroupIdentifier = getsize(header->gid);
            node->UserIdentifier = getsize(header->uid);
            node->IndexNode = i;

            switch (header->typeflag[0])
            {
            case REGULAR_FILE:
                node->Flags = NodeFlags::FILE;
                break;
            case SYMLINK:
                node->Flags = NodeFlags::SYMLINK;
                break;
            case DIRECTORY:
                node->Flags = NodeFlags::DIRECTORY;
                break;
            case CHARDEV:
                node->Flags = NodeFlags::CHARDEVICE;
                break;
            case BLOCKDEV:
                node->Flags = NodeFlags::BLOCKDEVICE;
                break;
            default:
                warn("Unknown type: %d", header->typeflag[0]);
                break;
            }
            Address += ((size / 512) + 1) * 512;
            if (size % 512)
                Address += 512;
        }
    }

    USTAR::~USTAR() { warn("Destroyed"); }
}
