#include <filesystem.h>

#include <bootscreen.h>

namespace FileSystem
{
    FileSystemNode *NetworkRootNode = nullptr;
    static uint64_t NetworkNodeIndexNodeCount = 0;

    FileSystemNode *Network::AddNetworkCard(struct FileSystemOpeations *Operator, uint64_t Mode, string Name, int Flags)
    {
        trace("Adding %s to file system", Name);
        // FileSystemNode *newNode = vfs->Create(NetworkRootNode, Name);

        FileSystemNode *newNode = new FileSystemNode;
        strcpy(newNode->Name, Name);
        newNode->IndexNode = NetworkNodeIndexNodeCount++;
        newNode->Mode = Mode;
        newNode->Operator = Operator;
        newNode->Flags = Flags;
        NetworkRootNode->Children.push_back(newNode);
        return newNode;
    }

    Network::Network()
    {
        trace("Initializing network file system...");
        NetworkRootNode = vfs->Create(nullptr, "/system/net");
        NetworkRootNode->Flags = NodeFlags::FS_DIRECTORY;
        NetworkRootNode->Mode = 0755;
        BS->IncreaseProgres();
    }

    Network::~Network()
    {
    }
}
