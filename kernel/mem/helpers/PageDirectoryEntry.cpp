#include <heap.h>

using namespace VMM;

void PageDirectoryEntry::AddFlags(uint64_t Flag)
{
    uint64_t BitSelector = (uint64_t)1 << Flag;
    this->Value |= BitSelector;
}

void PageDirectoryEntry::RemoveFlags(uint64_t Flag)
{
    uint64_t BitSelector = (uint64_t)1 << Flag;
    this->Value &= ~BitSelector;
}

void PageDirectoryEntry::ClearFlags()
{
    this->Value = 0;
}

void PageDirectoryEntry::SetFlag(uint64_t Flag, bool Enabled)
{
    uint64_t BitSelector = (uint64_t)1 << Flag;
    this->Value &= ~BitSelector;
    if (Enabled)
    {
        this->Value |= BitSelector;
    }
}

uint64_t PageDirectoryEntry::GetFlag()
{
    return this->Value;
}

bool PageDirectoryEntry::GetFlag(uint64_t Flag)
{
    uint64_t BitSelector = (uint64_t)1 << Flag;
    return (this->Value & BitSelector) > 0 ? true : false;
}

uint64_t PageDirectoryEntry::GetAddress()
{
    return (this->Value & 0x000FFFFFFFFFF000) >> 12;
}

void PageDirectoryEntry::SetAddress(uint64_t Address)
{
    Address &= 0x000000FFFFFFFFFF;
    this->Value &= 0xFFF0000000000FFF;
    this->Value |= (Address << 12);
}
