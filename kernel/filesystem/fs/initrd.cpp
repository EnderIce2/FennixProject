#include <filesystem.h>

namespace FileSystem
{
    ReadFSFunction(Initrd_Read)
    {
        return 0;
    }
    
    Initrd::Initrd(uint64_t Address)
    {
        InitrdHeader *Header = (InitrdHeader *)Address;
        // TODO: Implement this function
    }

    Initrd::~Initrd()
    {
    }
}
