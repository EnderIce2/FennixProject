long *stckptr;
static char *argv[20];
extern int main(int argc, char **argv, ...);
void _start(void)
{
    __asm__("mov %%rsp, %%rcx"
            : "=c"(stckptr)
            :);
    int argc = *((int *)(stckptr + 1));
    if (argc > 0 && argc < 5)
    {
        for (int i = 0; i < argc; i++)
            argv[i] = ((char *)(stckptr + 2 + 8 * i));
        main(argc, argv, (void *)0);
    }
    else
        main(0, (void *)0, (void *)0);
    unsigned long ret;
    __asm__ volatile("syscall"
                     : "=a"(ret)
                     : "a"(1), "D"(0)
                     : "rcx", "r11", "memory");
    if (ret == 0)
        while (1)
            ;
}
