#include "shutdown.hpp"
#include <system.h>

int main(int argc, char *argv[])
{
    WriteSysDebugger("Shutting down...\n");
    syscall_shutdown();
    return 0;
}
