#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    static inline uint8_t inportb(uint16_t Port)
    {
        uint8_t result;
        asm("in %%dx, %%al"
            : "=a"(result)
            : "d"(Port));
        return result;
    }

    static inline uint16_t inportw(uint16_t Port)
    {
        uint16_t result;
        asm("in %%dx, %%ax"
            : "=a"(result)
            : "d"(Port));
        return result;
    }

    static inline uint32_t inportl(uint16_t Port)
    {
        uint32_t result;
        asm volatile("inl %1, %0"
                     : "=a"(result)
                     : "dN"(Port));
        return result;
    }

    static inline void outportb(uint16_t Port, uint8_t Data)
    {
        asm volatile("out %%al, %%dx"
                     :
                     : "a"(Data), "d"(Port));
    }

    static inline void outportw(uint16_t Port, uint16_t Data)
    {
        asm volatile("out %%ax, %%dx"
                     :
                     : "a"(Data), "d"(Port));
    }

    static inline void outportl(uint16_t Port, uint32_t Data)
    {
        asm volatile("outl %1, %0"
                     :
                     : "dN"(Port), "a"(Data));
    }

    static inline uint8_t mmioin8(uint64_t Address)
    {
        asm volatile("" ::
                         : "memory");
        uint8_t value = *(volatile uint8_t *)Address;
        asm volatile("" ::
                         : "memory");
        return value;
    }

    static inline uint16_t mmioin16(uint64_t Address)
    {
        asm volatile("" ::
                         : "memory");
        uint16_t value = *(volatile uint16_t *)Address;
        asm volatile("" ::
                         : "memory");
        return value;
    }

    static inline uint32_t mmioin32(uint64_t Address)
    {
        asm volatile("" ::
                         : "memory");
        uint32_t value = *(volatile uint32_t *)Address;
        asm volatile("" ::
                         : "memory");
        return value;
    }

    static inline uint64_t mmioin64(uint64_t Address)
    {
        asm volatile("" ::
                         : "memory");
        uint64_t value = *(volatile uint64_t *)Address;
        asm volatile("" ::
                         : "memory");
        return value;
    }

    static inline void mmioout8(uint64_t Address, uint8_t Data)
    {
        asm volatile("" ::
                         : "memory");
        *(volatile uint8_t *)Address = Data;
        asm volatile("" ::
                         : "memory");
    }

    static inline void mmioout16(uint64_t Address, uint16_t Data)
    {
        asm volatile("" ::
                         : "memory");
        *(volatile uint16_t *)Address = Data;
        asm volatile("" ::
                         : "memory");
    }

    static inline void mmioout32(uint64_t Address, uint32_t Data)
    {
        asm volatile("" ::
                         : "memory");
        *(volatile uint32_t *)Address = Data;
        asm volatile("" ::
                         : "memory");
    }

    static inline void mmioout64(uint64_t Address, uint64_t Data)
    {
        asm volatile("" ::
                         : "memory");
        *(volatile uint64_t *)Address = Data;
        asm volatile("" ::
                         : "memory");
    }

    static inline void mmoutb(void *Address, uint8_t value)
    {
        asm volatile("mov %1, %0"
                     : "=m"((*(uint8_t *)(Address)))
                     : "r"(value)
                     : "memory");
    }

    static inline void mmoutw(void *Address, uint16_t value)
    {
        asm volatile("mov %1, %0"
                     : "=m"((*(uint16_t *)(Address)))
                     : "r"(value)
                     : "memory");
    }

    static inline void mmoutl(void *Address, uint32_t value)
    {
        asm volatile("mov %1, %0"
                     : "=m"((*(uint32_t *)(Address)))
                     : "r"(value)
                     : "memory");
    }

    static inline void mmoutq(void *Address, uint64_t value)
    {
        asm volatile("mov %1, %0"
                     : "=m"((*(uint64_t *)(Address)))
                     : "r"(value)
                     : "memory");
    }

    static inline uint8_t mminb(void *Address)
    {
        uint8_t ret;
        asm volatile("mov %1, %0"
                     : "=r"(ret)
                     : "m"((*(uint8_t *)(Address)))
                     : "memory");
        return ret;
    }

    static inline uint16_t mminw(void *Address)
    {
        uint16_t ret;
        asm volatile("mov %1, %0"
                     : "=r"(ret)
                     : "m"((*(uint16_t *)(Address)))
                     : "memory");
        return ret;
    }

    static inline uint32_t mminl(void *Address)
    {
        uint32_t ret;
        asm volatile("mov %1, %0"
                     : "=r"(ret)
                     : "m"((*(uint32_t *)(Address)))
                     : "memory");
        return ret;
    }

    static inline uint64_t mminq(void *Address)
    {
        uint64_t ret;
        asm volatile("mov %1, %0"
                     : "=r"(ret)
                     : "m"((*(uint64_t *)(Address)))
                     : "memory");
        return ret;
    }

#ifdef __cplusplus
}
#endif

#define inb(Port) inportb(Port)
#define inw(Port) inportw(Port)
#define inl(Port) inportl(Port)
#define outb(Port, Data) outportb(Port, Data)
#define outw(Port, Data) outportw(Port, Data)
#define outl(Port, Data) outportl(Port, Data)
