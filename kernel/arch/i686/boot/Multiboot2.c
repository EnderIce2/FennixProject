#include <cputables.h>

extern int CheckCPUID();
extern int CheckLongMode();
extern void CallHigherHalf();
extern void LoadPageDirectory(unsigned int[]);
extern void EnablePaging();
extern uint64_t _kernel_start, _kernel_end;

__attribute__((no_stack_protector, section(".multiboot.text"))) void _start()
{
    for (int i = 0; i < 2000; i++)
    {
        ((unsigned int *)0xb8000)[i * 2] = ' ';
        ((unsigned int *)0xb8000)[i * 2 + 1] = 0;
    }

    char pleasewait[16] = "Please wait...";
    char longmodewarn[72] = "Warning: Long mode it's available! Consider using the correct OS image.";
    char notimplemented[21] = "Not implemented yet!";
    char error[14] = "Not Supported.";

    for (int i = 0; i < 16; i++)
        ((unsigned int *)0xb8000)[i] = (0x7 << 8) | pleasewait[i];

    if (CheckCPUID())
        if (CheckLongMode())
            for (int i = 0; i < 72; i++)
                ((unsigned int *)0xb8000)[i] = (0x7 << 8) | longmodewarn[i];

    unsigned int PD[1024] __attribute__((aligned(4096)));
    unsigned int PT[1024] __attribute__((aligned(4096)));
    unsigned int KPT[1024] __attribute__((aligned(4096)));

    for (int i = 0; i < 1024; i++)
        PD[i] = 0x00000002;

    for (int i = 0; i < 1024; i++)
        PT[i] = (i * 0x1000) | 3;

    for (uint64_t i = &_kernel_start; i < (uint64_t)&_kernel_end; i += 0x1000)
        KPT[(i - (uint64_t)&_kernel_start) / 0x1000] = i | 3;

    // for (int i = 1024; i < 2048; i++)
    // {
    //     KPT[i >> 12 & 0x03FF] = (i * 0x1000) | 3;
    // }

    PD[0] = ((unsigned int)PT) | 3;
    PD[1] = ((unsigned int)KPT) | 3;

    LoadPageDirectory(PD);
    EnablePaging();

    // TODO: map kernel to higher half memory
    // CallHigherHalf();

    for (int i = 0; i < 21; i++)
        ((unsigned int *)0xb8000)[i] = (0x7 << 8) | notimplemented[i];

    __asm__ volatile("1:\n"
                     "cli\n"
                     "hlt\n"
                     "jmp 1b\n");

    __asm__ volatile(".section .multiboot.text\n"
                     ".global LoadPageDirectory\n"
                     "LoadPageDirectory:\n"
                     "push %ebp\n"
                     "mov %esp, %ebp\n"
                     "mov 8(%esp), %eax\n"
                     "mov %eax, %cr3\n"
                     "mov %ebp, %esp\n"
                     "pop %ebp\n"
                     "ret\n");

    __asm__ volatile(".section .multiboot.text\n"
                     ".global EnablePaging\n"
                     "EnablePaging:\n"
                     "push %ebp\n"
                     "mov %esp, %ebp\n"
                     "mov %cr0, %eax\n"
                     "or $0x80010000, %eax\n"
                     "mov %eax, %cr0\n"
                     "mov %ebp, %esp\n"
                     "pop %ebp\n"
                     "ret\n");

    // __asm__ volatile(".section .bootstrap_stack, \"aw\", @nobits\n"
    //                  "BootstrapStackBottom:\n"
    //                  ".skip 16384\n"
    //                  "BootstrapStackTop:\n"

    //                  ".section .multiboot.text\n"
    //                  "CallHigherHalf:\n"
    //                  "lea 4f, %ecx\n"
    //                  "jmp *%ecx\n"

    //                  ".section .text\n"
    //                  "4:\n"
    //                  "movl %cr3, %ecx\n"
    //                  "movl %ecx, %cr3\n"
    //                  "mov $BootstrapStackTop, %esp\n"
    //                  "call kernel_entry\n"
    //                  "cli\n"
    //                  "1:\n"
    //                  "hlt\n"
    //                  "jmp 1b\n");
}
