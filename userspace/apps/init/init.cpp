#include <types.h>

int main(int argc, char **argv)
{
    asm volatile("int $0x1");
    return 0;
}
