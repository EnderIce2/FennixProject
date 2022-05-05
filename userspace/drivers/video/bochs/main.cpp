#include <driver.h>

DRIVER_ENTRY
{
}

DRIVER = {
    .Name = "Bochs Video Driver",
    .Type = TYPE_VIDEO,
    .Bind = {BIND_NULL}};
