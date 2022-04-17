#include <filesystem.h>
#include <rand.h>

namespace FileSystem
{
    ReadFSFunction(Random_Read)
    {
        uint64_t s = 0;
        while (s < Size)
        {
            Buffer[s] = (*rand64()) % 0xFF;
            Offset++;
            s++;
        }
        return Size;
    }

    WriteFSFunction(Random_Write) { return Size; }

    FileSystemOpeations random = {
        .Name = "Random",
        .Read = &Random_Read,
        .Write = &Random_Write};

    Random::Random()
    {
        devfs->AddFileSystem(&random, 0444, "random", FileSystem::CHARDEVICE);
    }

    Random::~Random() { warn("Destroyed"); }
}
