#include <filesystem.h>
#include "../drivers/serial.h"

namespace FileSystem
{
    ReadFSFunction(Serial_Read)
    {
        if (Size < 1)
            return 0;
        memset(Buffer, 0x00, 1);
        uint64_t received = 0;
        while (received < Size)
        {
            // TODO: implement for other COM's
            Buffer[received] = read_serial(COM1);
            received++;
        }
        return received;
    }

    WriteFSFunction(Serial_Write)
    {
        uint64_t sent = 0;
        while (sent < Size)
        {
            // TODO: implement for other COM's
            write_serial(COM1, Buffer[sent]);
            sent++;
        }
        return Size;
    }

    FileSystemOpeations serial = {
        .Name = "Serial Port 1",
        .Read = Serial_Read,
        .Write = Serial_Write};

    Serial::Serial()
    {
        devfs->AddFileSystem(&serial, 0666, "com1", FileSystem::PIPE); // ? is really a pipe or..? not sure how to categorise this
    }

    Serial::~Serial() { warn("Destroyed"); }
}
