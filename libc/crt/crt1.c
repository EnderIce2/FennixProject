long *stckptr;
static char *argv[20];
extern int main(int argc, char **argv);

__attribute__((naked)) static inline unsigned long crt1_syscall(long call, ...)
{
    __asm__ volatile("push %rbp\n"
                     "mov %rsp, %rbp\n"
                     "mov %rdi, %rax\n"
                     "mov %rsi, %rdi\n"
                     "mov %rdx, %rsi\n"
                     "mov %rcx, %rdx\n"
                     "mov %r8, %r10\n"
                     "mov %r9, %r8\n"
                     "mov 8(%rsp), %r9\n"
                     "syscall\n"
                     "mov %rbp, %rsp\n"
                     "pop %rbp\n"
                     "ret");
}

void _start()
{
    __asm__("mov %%rsp, %%rcx"
            : "=c"(stckptr)
            :);
    int argc = *((int *)(stckptr + 1));
    if (argc > 0 && argc < 5)
    {
        for (int i = 0; i < argc; i++)
            argv[i] = ((char *)(stckptr + 2 + 8 * i));
        main(argc, argv);
    }
    else
        main(0, (void *)0);
    if (crt1_syscall(1, 0) == 0)
        while (1)
            ;
}
