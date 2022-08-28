#include <io.h>
#include <syscalls.h>

int main(int argc, char *argv[])
{
    syscall_dbg(0x3F8, (char *)"[DriverManager] Initialized\n");
        while (1)
            ;
    return 1;
}
