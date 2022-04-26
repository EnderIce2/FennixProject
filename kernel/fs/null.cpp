#include <filesystem.h>

namespace FileSystem
{
    ReadFSFunction(Null_Read)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x0, Size);
        return Size;
    }

    WriteFSFunction(Null_Write) { return Size; }

    FileSystemOpeations null = {
        .Name = "Null",
        .Read = Null_Read,
        .Write = Null_Write};

    Null::Null() { devfs->AddFileSystem(&null, 0666, "null", NodeFlags::FS_CHARDEVICE); }

    Null::~Null() { warn("Destroyed"); }
}
