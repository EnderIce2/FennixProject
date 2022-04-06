#include <io.h>
#include <asm.h>

uint8_t inportb(uint16_t Port)
{
    uint8_t result;
    asm("in %%dx, %%al"
        : "=a"(result)
        : "d"(Port));
    return result;
}

void outportb(uint16_t Port, uint8_t Data)
{
    asm volatile("out %%al, %%dx"
                 :
                 : "a"(Data), "d"(Port));
}

uint16_t inportw(uint16_t Port)
{
    uint16_t result;
    asm("in %%dx, %%ax"
        : "=a"(result)
        : "d"(Port));
    return result;
}

void outportw(uint16_t Port, uint16_t Data)
{
    asm volatile("out %%ax, %%dx"
                 :
                 : "a"(Data), "d"(Port));
}

uint32_t inportl(uint16_t Port)
{
    uint32_t result;
    asm volatile("inl %1, %0"
                 : "=a"(result)
                 : "dN"(Port));
    return result;
}

void outportl(uint16_t Port, uint32_t Data)
{
    asm volatile("outl %1, %0"
                 :
                 : "dN"(Port), "a"(Data));
}

void mmioout8(uint64_t Address, uint8_t Data)
{
    mem_barrier();
    *(volatile uint8_t *)Address = Data;
    mem_barrier();
}

void mmioout16(uint64_t Address, uint16_t Data)
{
    mem_barrier();
    *(volatile uint16_t *)Address = Data;
    mem_barrier();
}

void mmioout32(uint64_t Address, uint32_t Data)
{
    mem_barrier();
    *(volatile uint32_t *)Address = Data;
    mem_barrier();
}

void mmioout64(uint64_t Address, uint64_t Data)
{
    mem_barrier();
    *(volatile uint64_t *)Address = Data;
    mem_barrier();
}

uint8_t mmioin8(uint64_t Address)
{
    mem_barrier();
    uint8_t value = *(volatile uint8_t *)Address;
    mem_barrier();
    return value;
}

uint16_t mmioin16(uint64_t Address)
{
    mem_barrier();
    uint16_t value = *(volatile uint16_t *)Address;
    mem_barrier();
    return value;
}

uint32_t mmioin32(uint64_t Address)
{
    mem_barrier();
    uint32_t value = *(volatile uint32_t *)Address;
    mem_barrier();
    return value;
}

uint64_t mmioin64(uint64_t Address)
{
    mem_barrier();
    uint64_t value = *(volatile uint64_t *)Address;
    mem_barrier();
    return value;
}

void mmoutb(void *Address, uint8_t value)
{
    asm volatile("mov %1, %0"
                 : "=m"((*(uint8_t *)(Address)))
                 : "r"(value)
                 : "memory");
}

void mmoutw(void *Address, uint16_t value)
{
    asm volatile("mov %1, %0"
                 : "=m"((*(uint16_t *)(Address)))
                 : "r"(value)
                 : "memory");
}

void mmoutl(void *Address, uint32_t value)
{
    asm volatile("mov %1, %0"
                 : "=m"((*(uint32_t *)(Address)))
                 : "r"(value)
                 : "memory");
}

void mmoutq(void *Address, uint64_t value)
{
    asm volatile("mov %1, %0"
                 : "=m"((*(uint64_t *)(Address)))
                 : "r"(value)
                 : "memory");
}

uint8_t mminb(void *Address)
{
    uint8_t ret;
    asm volatile("mov %1, %0"
                 : "=r"(ret)
                 : "m"((*(uint8_t *)(Address)))
                 : "memory");
    return ret;
}

uint16_t mminw(void *Address)
{
    uint16_t ret;
    asm volatile("mov %1, %0"
                 : "=r"(ret)
                 : "m"((*(uint16_t *)(Address)))
                 : "memory");
    return ret;
}

uint32_t mminl(void *Address)
{
    uint32_t ret;
    asm volatile("mov %1, %0"
                 : "=r"(ret)
                 : "m"((*(uint32_t *)(Address)))
                 : "memory");
    return ret;
}

uint64_t mminq(void *Address)
{
    uint64_t ret;
    asm volatile("mov %1, %0"
                 : "=r"(ret)
                 : "m"((*(uint64_t *)(Address)))
                 : "memory");
    return ret;
}