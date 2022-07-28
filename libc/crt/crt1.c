// TODO: https://stackoverflow.com/a/2724377/9352057

long *stckptr;
static char *argv[20];
extern int main(int argc, char *argv[]);
extern void __cxa_finalize(void *f);

void _start()
{
    __asm__("mov %%rsp, %%rcx"
            : "=c"(stckptr)
            :);
    int argc = *((int *)(stckptr + 1));
    int mainret = 0;
    if (argc > 0 && argc < 5)
    {
        for (int i = 0; i < argc; i++)
            argv[i] = ((char *)(stckptr + 2 + 8 * i));
        mainret = main(argc, argv);
    }
    else
        mainret = main(0, (void *)0);

    __cxa_finalize(0);
    unsigned long syscall_return = 0;
    __asm__ __volatile__(
        "pushq %%r11\n"
        "pushq %%rcx\n"
        "syscall\n"
        "popq %%rcx\n"
        "popq %%r11\n"
        : "=a"(syscall_return)
        : "a"(1), "b"(mainret)
        : "memory");

    while (1)
        ;
}
