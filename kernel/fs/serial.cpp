#include <filesystem.h>
#include "../drivers/serial.h"

namespace FileSystem
{
    ReadFSFunction(Serial_Read1)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x00, 1);
        uint64_t received = 0;
        while (received < Size)
        {
            Buffer[received] = read_serial(COM1);
            received++;
        }
        return received;
    }

    WriteFSFunction(Serial_Write1)
    {
        uint64_t sent = 0;
        while (sent < Size)
        {
            write_serial(COM1, Buffer[sent]);
            sent++;
        }
        return Size;
    }

    ReadFSFunction(Serial_Read2)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x00, 1);
        uint64_t received = 0;
        while (received < Size)
        {
            Buffer[received] = read_serial(COM2);
            received++;
        }
        return received;
    }

    WriteFSFunction(Serial_Write2)
    {
        uint64_t sent = 0;
        while (sent < Size)
        {
            write_serial(COM2, Buffer[sent]);
            sent++;
        }
        return Size;
    }

    ReadFSFunction(Serial_Read3)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x00, 1);
        uint64_t received = 0;
        while (received < Size)
        {
            Buffer[received] = read_serial(COM3);
            received++;
        }
        return received;
    }

    WriteFSFunction(Serial_Write3)
    {
        uint64_t sent = 0;
        while (sent < Size)
        {
            write_serial(COM3, Buffer[sent]);
            sent++;
        }
        return Size;
    }

    ReadFSFunction(Serial_Read4)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x00, 1);
        uint64_t received = 0;
        while (received < Size)
        {
            Buffer[received] = read_serial(COM4);
            received++;
        }
        return received;
    }

    WriteFSFunction(Serial_Write4)
    {
        uint64_t sent = 0;
        while (sent < Size)
        {
            write_serial(COM4, Buffer[sent]);
            sent++;
        }
        return Size;
    }

    FileSystemOpeations serial1 = {
        .Name = "Serial Port 1",
        .Read = Serial_Read1,
        .Write = Serial_Write1};

    FileSystemOpeations serial2 = {
        .Name = "Serial Port 2",
        .Read = Serial_Read2,
        .Write = Serial_Write2};

    FileSystemOpeations serial3 = {
        .Name = "Serial Port 3",
        .Read = Serial_Read3,
        .Write = Serial_Write3};

    FileSystemOpeations serial4 = {
        .Name = "Serial Port 4",
        .Read = Serial_Read4,
        .Write = Serial_Write4};

    Serial::Serial()
    {
        devfs->AddFileSystem(&serial1, 0666, "com1", NodeFlags::FS_PIPE); // ? is really a pipe or..? not sure how to categorise this
        devfs->AddFileSystem(&serial2, 0666, "com2", NodeFlags::FS_PIPE); // ? is really a pipe or..? not sure how to categorise this
        devfs->AddFileSystem(&serial3, 0666, "com3", NodeFlags::FS_PIPE); // ? is really a pipe or..? not sure how to categorise this
        devfs->AddFileSystem(&serial4, 0666, "com4", NodeFlags::FS_PIPE); // ? is really a pipe or..? not sure how to categorise this
    }

    Serial::~Serial() { warn("Destroyed"); }
}
