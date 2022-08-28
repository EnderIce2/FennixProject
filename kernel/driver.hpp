#pragma once
#include <filesystem.h>
#include <types.h>
#include <driver.h>

namespace Driver
{
    class KernelDriver
    {
    private:
        void *DrvMgrProc;
        void *DrvMgrThrd;

    public:
        uint64_t LoadKernelDriverFromFile(FileSystem::FileSystemNode *Node);
        KernelDriver();
        ~KernelDriver();
    };
}

extern Driver::KernelDriver *kdrv;
