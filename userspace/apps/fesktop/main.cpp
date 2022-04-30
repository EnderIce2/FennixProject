#include <syscalls.h>

void WatchDesktop()
{
    while (1)
    {
    }
}

void WatchTaskBar()
{
    while (1)
    {
    }
}

int main(int argc, char **argv)
{
    // syscall_createThread((uint64_t)WatchDesktop);
    // syscall_createThread((uint64_t)WatchTaskBar);
    while (1)
        ;
    return 0;
}
