#include <driver.h>

DRIVER_ENTRY
{
    return 0;
}

DRIVER = {
    .Name = "VirtIO Video Driver",
    .Type = TYPE_VIDEO,
    .Bind = {BIND_NULL}};
