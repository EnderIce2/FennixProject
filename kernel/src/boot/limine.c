#include "limine.h"

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0};

bool init_limine(GlobalBootParams *params)
{
    if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1)
        return false;
    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, "Please wait... ", 15);
    terminal_request.response->write(terminal, "Limine protocol is not supported yet", 36);
    return false; // TODO: implement support for limine protocol
}

bool detect_limine()
{
    if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1)
        return false;
    else
        return true;
}
