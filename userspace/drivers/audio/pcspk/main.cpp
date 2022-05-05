#include <driver.h>

DRIVER_ENTRY
{
}

DRIVER = {
    .Name = "PC Speaker Driver",
    .Type = TYPE_AUDIO,
    .Bind = {BIND_NULL}};
