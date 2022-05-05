#include <driver.h>

DRIVER_ENTRY
{
}

DRIVER = {
    .Name = "VirtIO Video Driver",
    .Type = TYPE_VIDEO,
    .Bind = {BIND_NULL}};
