#include <driver.h>

DRIVER_ENTRY
{
    return 0;
}

DRIVER = {
    .Name = "AC97 Audio Driver",
    .Type = TYPE_AUDIO,
    .Bind = {BIND_NULL}};
