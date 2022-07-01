#include <driver.h>

DRIVER_ENTRY
{
    return 0;
}

DRIVER = {
    .Name = "PS/2 Mouse Driver",
    .Type = TYPE_MOUSE,
    .Bind =
        {
            .Bind = BIND_INTERRUPT,
            .Interrupt =
                {
                    .Vector = 0x2C}}};
