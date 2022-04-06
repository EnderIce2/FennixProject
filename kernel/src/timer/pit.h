#ifndef __FENNIX_KERNEL_APIC_PIT_H__
#define __FENNIX_KERNEL_APIC_PIT_H__

#include <types.h>

// TODO: document defines
#define PIT_COUNTER0 0x40
#define PIT_CMD 0x43

#define CMD_MODE0 0x00
#define CMD_MODE1 0x02
#define CMD_MODE2 0x04
#define CMD_MODE3 0x06
#define CMD_MODE4 0x08
#define CMD_MODE5 0x0a
#define CMD_COUNTER0 0x00
#define CMD_COUNTER1 0x40
#define CMD_COUNTER2 0x80
#define CMD_READBACK 0xc0
#define CMD_LATCH 0x00
#define CMD_RW_LOW 0x10
#define CMD_RW_HI 0x20
#define CMD_RW_BOTH 0x30
#define CMD_BINARY 0x00
#define CMD_BCD 0x01

extern bool PIC_initialized;
uint32_t get_freq();
void init_pit();
void pit_wait(uint32_t miliseconds);
uint64_t pit_get_uptime();

#endif // !__FENNIX_KERNEL_APIC_PIT_H__