#include <heap.h>

using namespace VMM;

PageTableManager::PageTableManager(PageTable *PML4Address)
{
    this->PML4 = PML4Address;
}
