#include <filesystem.h>

namespace FileSystem
{
    ReadFSFunction(Zero_Read)
    {
        memset(Buffer, 0, Size);
        return 0;
    }

    WriteFSFunction(Zero_Write) { return Size; }

    FileSystemOpeations zero = {
        .Name = "Zero",
        .Read = Zero_Read,
        .Write = Zero_Write};

    Zero::Zero()
    {
        devfs->AddFileSystem(&zero, 0666, "zero", FileSystem::CHARDEVICE);
    }

    Zero::~Zero() { warn("Destroyed"); }
}
