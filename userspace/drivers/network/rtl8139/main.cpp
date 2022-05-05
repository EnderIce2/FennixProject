#include <driver.h>

DRIVER_ENTRY
{
}

DRIVER = {
    .Name = "Realtek-8139 Network Driver",
    .Type = TYPE_NETWORK,
    .Bind = {BIND_NULL}};
