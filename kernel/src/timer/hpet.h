#ifndef __FENNIX_KERNEL_HPET_H__
#define __FENNIX_KERNEL_HPET_H__

#include <types.h>

struct HPET
{
    uint64_t general_capabilities;
    uint64_t reserved;
    uint64_t general_configuration;
    uint64_t reserved2;
    uint64_t general_int_status;
    uint64_t reserved3;
    uint64_t reserved4[24];
    uint64_t main_counter_value;
    uint64_t reserved5;
};

extern bool HPET_initialized;
uint32_t get_clk();
uint64_t hpet_read_counter();
void hpet_uwait(uint64_t Microseconds);
void hpet_mwait(uint64_t Miliseconds);
void hpet_wait(uint64_t Seconds);
void init_HPET();

#endif // !__FENNIX_KERNEL_HPET_H__