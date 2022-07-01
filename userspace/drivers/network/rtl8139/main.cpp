#include <driver.h>

DRIVER_ENTRY
{
    return 0;
}

DRIVER = {
    .Name = "Realtek-8139 Network Driver",
    .Type = TYPE_NETWORK,
    .Bind = {BIND_NULL}};
