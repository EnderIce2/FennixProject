#include "../cmds.hpp"

#include "../monoton.hpp"
#include "../cwalk.h"

#include <system.h>
#include <alloc.h>

// TODO: should return the data instead
void Cat(File *CurrentPath, const char *Arguments)
{
    char *path = (char *)malloc(strlen(Arguments) + 1);
    cwk_path_normalize(Arguments, path, strlen(Arguments) + 1);
    bool success = true;
    File *node = (File *)syscall_FileOpenWithParent(path, CurrentPath);
    if (node->Status != FileStatus::OK)
    {
        mono->print("No such file or directory!");
        success = false;
    }
    else if (node->Status != FileStatus::OK)
    {
        WriteSysDebugger("%s node error %#x", node->Name, node->Flags);
        mono->print("Could not open file!");
        success = false;
    }
    if (success)
    {
        // TODO: I have to implement CTRL+C to cancel the reading operation
        switch (node->Flags)
        {
        case FS_FILE:
        case FS_CHARDEVICE:
        {
            uint64_t size = 64;
            if (node->Length)
                size = node->Length;
            uint8_t *txt = (uint8_t *)(calloc(size, sizeof(uint8_t)));
            if (size == 64)
            {
                size = syscall_FileRead(node, 0, txt, size);
                if (size == 0)
                    size = strlen((char *)txt);
            }
            else
                syscall_FileRead(node, 0, txt, size);
            for (uint64_t i = 0; i < size; i++)
                mono->printchar(txt[i]);
            free(txt);
            break;
        }
        case FS_PIPE:
        case FS_BLOCKDEVICE:
        {
            uint8_t *txt = (uint8_t *)(calloc(64, sizeof(uint8_t)));
            syscall_FileRead(node, 0, txt, 64);
            for (uint64_t i = 0; i < 64; i++)
            {
                if (txt[i] == '\0')
                    break;
                mono->printchar(txt[i]);
            }
            free(txt);
            break;
        }
        default:
            mono->print("Cannot read from file.");
            break;
        }
    }
    syscall_FileClose(node);
    free(path);
}
