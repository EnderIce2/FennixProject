#include "monoton.hpp"

#include "cwalk.h"

#include <convert.h>
#include <system.h>
#include <string.h>
#include <print.h>
#include <alloc.h>

MonotonLib::mtl *mono = nullptr;
File *CurrentPath = nullptr;
static char CurrentFullPath[255] = {'/', '\0'};
static char key_buffer[1024];

void PrintShellPrefix()
{
    mono->SetForegroundColor(0x6BFBF5);
    mono->print(usr());
    mono->SetForegroundColor(0xFFDF8F);
    mono->print((char *)"@");
    mono->SetForegroundColor(0x6BFBF5);
    mono->print((char *)"fennix");
    mono->SetForegroundColor(0xFFDF8F);
    mono->printchar(':');
    mono->SetForegroundColor(0xADFF54);
    mono->print(CurrentFullPath);
    mono->SetForegroundColor(0xFFDF8F);
    mono->print("$ ");
    mono->SetForegroundColor();
}

char *trimwhitespace(char *str)
{
    char *end;
    while (*str == ' ')
        str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ')
        end--;
    *(end + 1) = 0;
    return str;
}

void ParseBuffer(char *Buffer);

void loop()
{
    int backspacelimit = 0;
    while (1)
    {
        int key = GetLetterFromScanCode((uint8_t)syscall_getLastKeyboardScanCode());

        if (key != KEY_INVALID)
        {
            if (key == KEY_D_BACKSPACE)
            {
                if (backspacelimit > 0)
                {
                    mono->RemoveChar();
                    backspace(key_buffer);
                    backspacelimit--;
                }
            }
            else if (key == '\n')
            {
                ParseBuffer(key_buffer);
                backspacelimit = 0;
                key_buffer[0] = '\0';
            }
            else
            {
                append(key_buffer, key);
                mono->printchar(key);
                backspacelimit++;
            }
        }
    }
}

void ParseBuffer(char *Buffer)
{
    mono->printchar('\n');
    if (strcmp(Buffer, "help") == 0)
    {
        mono->print("Monoton Shell for Fennix\n");
        mono->print("------------------------\n");
        mono->print("help  clear  echo  ls  cat  cd  finfo\n");
        mono->print("For more information, use [command] --help");
    }
    else if (strcmp(Buffer, "clear") == 0)
    {
        mono->Clear();
    }
    else if (strncmp(Buffer, "echo", 4) == 0)
    {
        char *arg = trimwhitespace(Buffer + 4);
        mono->print(arg);
    }
    else if (strncmp(Buffer, "ls", 2) == 0)
    {
        char *arg = trimwhitespace(Buffer + 2);
        char *path = (char *)malloc(strlen(arg) + 1);
        cwk_path_normalize(arg, path, strlen(arg) + 1);
        bool success = true;
        File *node = (File *)syscall_FileOpenWithParent(path, CurrentPath);
        if (node->Status != FileStatus::OK)
        {
            mono->print("Node failed.");
            success = false;
        }
        if (!node)
        {
            mono->print("No such file or directory!");
            free(path);
            success = false;
        }
        if (node->Flags != FS_DIRECTORY &&
            node->Flags != FS_BLOCKDEVICE &&
            node->Flags != FS_MOUNTPOINT)
        {
            WriteSysDebugger("%s is not a directory (%d)\n", node->Name, node->Flags);
            mono->print(node->Name);
            mono->print(" is not a directory!");
            free(path);
            success = false;
        }
        if (success)
            free(path);

        if (success)
            for (uint64_t i = 0; i < syscall_FileChildrenSize(node); i++)
            {
                File *n = (File *)syscall_FileGetChildren(node, i);
                mono->print("  ");
                switch (n->Flags)
                {
                case FS_FILE:
                    mono->SetForegroundColor(0x66E8E6);
                    mono->print(n->Name);
                    break;
                case FS_DIRECTORY:
                    mono->SetForegroundColor(0x6689E8);
                    mono->print(n->Name);
                    break;
                case FS_CHARDEVICE:
                    mono->SetForegroundColor(0x91E866);
                    mono->print(n->Name);
                    break;
                case FS_BLOCKDEVICE:
                    mono->SetForegroundColor(0xE8CE66);
                    mono->print(n->Name);
                    break;
                case FS_PIPE:
                    mono->SetForegroundColor(0xBD66E8);
                    mono->print(n->Name);
                    break;
                case FS_SYMLINK:
                    mono->SetForegroundColor(0x2B2FFF);
                    mono->print(n->Name);
                    break;
                case FS_MOUNTPOINT:
                    mono->SetForegroundColor(0x2A87DE);
                    mono->print(n->Name);
                    break;
                default:
                    mono->SetForegroundColor(0xB3172E);
                    mono->print(n->Name);
                    break;
                }
                mono->SetForegroundColor();
                mono->SetBackgroundColor();
                syscall_FileClose(n);
            }
        syscall_FileClose(node);
    }
    else if (strncmp(Buffer, "cat", 3) == 0)
    {
        char *arg = trimwhitespace(Buffer + 2);
        char *path = (char *)malloc(strlen(arg) + 1);
        cwk_path_normalize(arg, path, strlen(arg) + 1);
        bool success = true;
        File *node = (File *)syscall_FileOpenWithParent(path, CurrentPath);
        if (!node)
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
            switch (node->Flags)
            {
            case FS_FILE:
            case FS_CHARDEVICE:
            {
                uint64_t size = 50;
                if (node->Length)
                    size = node->Length;
                char *txt = (char *)(calloc(size, sizeof(char)));
                syscall_FileRead(node, 0, txt, size);
                for (uint64_t i = 0; i < size; i++)
                    mono->printchar(txt[i]);
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
    else if (strncmp(Buffer, "cd", 2) == 0)
    {
        char *arg = trimwhitespace(Buffer + 2);

        if (isempty_1(arg))
            strcpy(arg, "/");

        char *path = (char *)malloc(strlen(arg) + 1);
        cwk_path_normalize(arg, path, strlen(arg) + 1);
        File *node = (File *)syscall_FileOpenWithParent(path, CurrentPath);
        bool success = true;
        if (!node)
        {
            mono->print("No such file directory!");
            syscall_FileClose(node);
            success = false;
        }
        else if (node->Flags != FS_DIRECTORY &&
                 node->Flags != FS_BLOCKDEVICE &&
                 node->Flags != FS_MOUNTPOINT)
        {
            mono->print(path);
            mono->print(" is not a directory!");
            syscall_FileClose(node);
            success = false;
        }
        else if (node->Status != FileStatus::OK)
        {
            syscall_FileClose(node);
            success = false;
        }
        if (success)
        {
            char *curpath = (char *)syscall_FileFullPath(node);
            strcpy(CurrentFullPath, curpath);
            if (isempty_1(CurrentFullPath))
                strcpy(CurrentFullPath, "/");
            CurrentPath = node;
        }
        free(path);
    }
    else if (strncmp(Buffer, "finfo", 5) == 0)
    {
        char *arg = trimwhitespace(Buffer + 2);
        char *path = (char *)malloc(strlen(arg) + 1);
        cwk_path_normalize(arg, path, strlen(arg) + 1);
        File *node = (File *)syscall_FileOpenWithParent(path, CurrentPath);
        if (!node)
        {
            mono->print("No such file or directory!");
        }
        else
        {
            char str[100];
            mono->print("File Info for ");
            char *fpath = (char *)syscall_FileFullPath(node);
            mono->print(fpath);
            mono->print("\nName: ");
            mono->print(node->Name);
            mono->print("\nStatus: 0x");
            itoa(node->Status, str, 16);
            mono->print(str);
            mono->print("\nIndex: ");
            itoa(node->IndexNode, str, 10);
            mono->print(str);
            mono->print("\nMask: ");
            itoa(node->Mask, str, 10);
            mono->print(str);
            mono->print("\nMode: ");
            itoa(node->Mode, str, 10);
            mono->print(str);
            mono->print("\nFlags: 0x");
            itoa(node->Flags, str, 16);
            mono->print(str);
            mono->print("\nUserID: ");
            itoa(node->UserIdentifier, str, 10);
            mono->print(str);
            mono->print("\nGroupID: ");
            itoa(node->GroupIdentifier, str, 10);
            mono->print(str);
            mono->print("\nAddress: 0x");
            itoa(node->Address, str, 16);
            mono->print(str);
            mono->print("\nLength: ");
            itoa(node->Length, str, 10);
            mono->print(str);
            mono->printchar('\n');
        }
        syscall_FileClose(node);
        free(path);
    }
    else
    {
        int tried = 0;
    FilesSearch:
        const char *searchpath[] = {
            "/system/",
            "/home/default/apps/",
            "/home/default/games/",
        };
        char filepath[256] = {'\0'};
        switch (tried)
        {
        case 0:
            strcpy(filepath, searchpath[0]);
            break;
        case 1:
            strcpy(filepath, searchpath[1]);
            break;
        case 2:
            strcpy(filepath, searchpath[2]);
            break;
        case 3:
            strcat(filepath, "/home/");
            strcat(filepath, usr());
            strcat(filepath, "/apps/");
            break;
        case 4:
            strcat(filepath, "/home/");
            strcat(filepath, usr());
            strcat(filepath, "/games/");
            break;
        default:
            break;
        }
        tried++;
        strcat(filepath, Buffer);
        WriteSysDebugger("[MonotonShell] Searching file %s...\n", filepath);
        File *f = (File *)syscall_FileOpen(filepath);
        if (f->Status != FileStatus::OK)
        {
            syscall_FileClose(f);
            if (tried < 5)
                goto FilesSearch;
            else
                f = (File *)syscall_FileOpenWithParent(Buffer, CurrentPath);
        }

        if (f->Status == FileStatus::OK)
        {
            if (!isempty_1(Buffer))
            {
                syscall_createProcess((char *)filepath, 0, 0);
                syscall_pushTask((uint64_t)&loop);
            }
            else
            {
                mono->print(Buffer);
                if (strlen(Buffer) > 1)
                    mono->print(": Command not found.");
            }
        }
        else
        {
            mono->print(Buffer);
            if (strlen(Buffer) > 1)
                mono->print(": Command not found.");
        }
        syscall_FileClose(f);
    }
    mono->printchar('\n');
    PrintShellPrefix();
}

static bool AlreadyInitialized = false;

int main(int argc, char *argv[])
{
    if (AlreadyInitialized)
    {
        WriteSysDebugger("[MonotonShell] Restored.\n");
        PrintShellPrefix();
        key_buffer[0] = '\0';
        loop();
    }
    WriteSysDebugger("[MonotonShell] Started.\n");
    // mono 1 - multi 2
    if (syscall_getScheduleMode() == 2)
    {
        WriteSysDebugger("[MonotonShell] Error! Program launched from multitasking mode.\n");
        while (1)
            ;
    }

    uint64_t address = syscall_displayAddress();
    // uint64_t width = syscall_displayWidth();
    uint64_t height = syscall_displayHeight();
    uint64_t ppsl = syscall_displayPixelsPerScanLine();
    for (int VerticalScanline = 0; VerticalScanline < height; VerticalScanline++)
    {
        uint64_t PixelPtrBase = address + ((ppsl * 4) * VerticalScanline);
        for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (ppsl * 4)); PixelPtr++)
            *PixelPtr = 0xFF000000;
    }

    // Default font: /system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf
    mono = new MonotonLib::mtl((char *)"/system/fonts/tamsyn-font-1.11/Tamsyn8x16b.psf");
    CurrentPath = (File *)syscall_FileOpen((char *)"/");

    InitLogin();
    mono->SetForegroundColor(0xDE2A39);
    mono->print((char *)"\n-- This shell is not fully implemented! --\n");
    PrintShellPrefix();

    AlreadyInitialized = true;

    loop();
    return 1;
}
