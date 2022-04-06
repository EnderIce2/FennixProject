#pragma once
#include <types.h>

START_EXTERNC

uint8_t inportb(uint16_t Port);
uint16_t inportw(uint16_t Port);
void outportb(uint16_t Port, uint8_t Data);
void outportw(uint16_t Port, uint16_t Data);
void outportl(uint16_t Port, uint32_t Data);
uint32_t inportl(uint16_t Port);

uint8_t mmioin648(uint64_t Address);
uint16_t mmioin6416(uint64_t Address);
uint32_t mmioin6432(uint64_t Address);
uint64_t mmioin6464(uint64_t Address);
void mmioout8(uint64_t Address, uint8_t Data);
void mmioout16(uint64_t Address, uint16_t Data);
void mmioout32(uint64_t Address, uint32_t Data);
void mmioout64(uint64_t Address, uint64_t Data);

void mmoutb(void *Address, uint8_t value);
void mmoutw(void *Address, uint16_t value);
void mmoutl(void *Address, uint32_t value);
void mmoutq(void *Address, uint64_t value);
uint8_t mminb(void *Address);
uint16_t mminw(void *Address);
uint32_t mminl(void *Address);
uint64_t mminq(void *Address);

END_EXTERNC

#define inb(Port) inportb(Port)
#define inw(Port) inportw(Port)
#define inl(Port) inportl(Port)
#define outb(Port, Data) outportb(Port, Data)
#define outw(Port, Data) outportw(Port, Data)
#define outl(Port, Data) outport(Port, Data)
