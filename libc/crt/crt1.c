// TODO: https://stackoverflow.com/a/2724377/9352057

extern int main(int argc, char *argv[], char *envp[]);
extern void __cxa_finalize(void *f);

void _start()
{
    // long argc = 0;
    // long argv = 0;
    // long envp = 0;

    // __asm__ __volatile__(
    //     "pushq %%r11\n"
    //     "pushq %%rcx\n"
    //     "syscall\n"
    //     "popq %%rcx\n"
    //     "popq %%r11\n"
    //     : "=a"(argc)
    //     : "a"(8)
    //     : "memory");
    // __asm__ __volatile__(
    //     "pushq %%r11\n"
    //     "pushq %%rcx\n"
    //     "syscall\n"
    //     "popq %%rcx\n"
    //     "popq %%r11\n"
    //     : "=a"(argv)
    //     : "a"(9)
    //     : "memory");
    // __asm__ __volatile__(
    //     "pushq %%r11\n"
    //     "pushq %%rcx\n"
    //     "syscall\n"
    //     "popq %%rcx\n"
    //     "popq %%r11\n"
    //     : "=a"(envp)
    //     : "a"(10)
    //     : "memory");

    // int mainret = main((int)argc, (char **)argv, (char **)envp);

    register long arg1 __asm__("rdi");
    register long arg2 __asm__("rsi");
    int mainret = main((int)arg1, (char **)arg2, (char **)0);

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
