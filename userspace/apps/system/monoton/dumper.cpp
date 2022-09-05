#include "dumper.hpp"

#include "../../../libs/monoton/monotonlib.h"

extern MonotonLib::mtl *mono;

#define DumperPrint(m, ...) mono->printf(m, ##__VA_ARGS__)

void DumpData(const char *Description, void *Address, unsigned long Length)
{
    mono->print("-------------------------------------------------------------------------\n");
    unsigned char *AddressChar = (unsigned char *)Address;
    unsigned char Buffer[17];
    unsigned long Iterate;

    if (Description != nullptr)
        DumperPrint("%s:\n", Description);

    for (Iterate = 0; Iterate < Length; Iterate++)
    {
        if ((Iterate % 16) == 0)
        {
            if (Iterate != 0)
                DumperPrint("  %s\n", Buffer);
            DumperPrint("  %04x ", Iterate);
        }

        DumperPrint(" %02x", AddressChar[Iterate]);

        if ((AddressChar[Iterate] < 0x20) || (AddressChar[Iterate] > 0x7e))
            Buffer[Iterate % 16] = '.';
        else
            Buffer[Iterate % 16] = AddressChar[Iterate];

        Buffer[(Iterate % 16) + 1] = '\0';
    }

    while ((Iterate % 16) != 0)
    {
        DumperPrint("   ");
        Iterate++;
    }

    DumperPrint("  %s\n", Buffer);
    mono->print("-------------------------------------------------------------------------\n");
}
