#include <io.h>
#include <syscalls.h>

int main(int argc, char **argv)
{
    return syscall(_DebugMessage, 1, (long long)"Hello, world!",  (long long)__FILE__, __LINE__, (long long)__FUNCTION__);
    // asm volatile("int $0x1");
}
