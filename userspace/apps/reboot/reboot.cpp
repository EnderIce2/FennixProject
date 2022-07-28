#include "reboot.hpp"

int main(int argc, char *argv[])
{
    syscall_reboot();
    return 0;
}
