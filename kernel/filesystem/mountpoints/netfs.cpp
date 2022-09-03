#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *NetworkRootNode = nullptr;

    FileSystemNode *Network::AddNetworkCard(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        FileSystemNode *newNode = vfs->Create(NetworkRootNode, Name);
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        return newNode;
    }

    Network::Network()
    {
        trace("Initializing network file system...");
        NetworkRootNode = vfs->Create(nullptr, "/system/net");
        NetworkRootNode->Flags = NodeFlags::FS_MOUNTPOINT;
        NetworkRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Network::~Network()
    {
    }
}
