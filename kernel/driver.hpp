#pragma once
#include <types.h>
#include <filesystem.h>

namespace Driver
{
    class KernelDriver
    {
    public:
        uint64_t LoadKernelDriver(FileSystem::FileSystemNode *Node);
        KernelDriver();
        ~KernelDriver();
    };
}

extern Driver::KernelDriver *kdrv;
