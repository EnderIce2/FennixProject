#include "shutdown.hpp"

int main(int argc, char **argv)
{
    syscall_shutdown();
    return 0;
}
