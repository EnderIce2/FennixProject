#include <heap.h>

using namespace VMM;

PageMapIndexer::PageMapIndexer(uint64_t VirtualAddress)
{
    PDP_i = (VirtualAddress & ((uint64_t)0x1FF << 39)) >> 39;
    PD_i = (VirtualAddress & ((uint64_t)0x1FF << 30)) >> 30;
    PT_i = (VirtualAddress & ((uint64_t)0x1FF << 21)) >> 21;
    P_i = (VirtualAddress & ((uint64_t)0x1FF << 12)) >> 12;
}
