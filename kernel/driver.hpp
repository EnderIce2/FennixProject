#pragma once
#include <filesystem.h>
#include <types.h>

namespace Driver
{
    class KernelDriver
    {
    private:
        void *DrvMgrProc;
        FileSystem::FileSystemNode *DriverNode;

    public:
        uint64_t LoadKernelDriver(FileSystem::FileSystemNode *Node);
        KernelDriver();
        ~KernelDriver();
    };
}

extern Driver::KernelDriver *kdrv;
