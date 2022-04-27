#include <heap.h>

using namespace VMM;

void PageDirectoryEntry::AddFlag(uint64_t Flag) { this->Value.raw |= Flag; }

void PageDirectoryEntry::RemoveFlags(uint64_t Flag) { this->Value.raw &= ~Flag; }

void PageDirectoryEntry::ClearFlags() { this->Value.raw = 0; }

void PageDirectoryEntry::SetFlag(uint64_t Flag, bool Enabled)
{
    this->Value.raw &= ~Flag;
    if (Enabled)
        this->Value.raw |= Flag;
}

uint64_t PageDirectoryEntry::GetFlag() { return this->Value.raw; }

bool PageDirectoryEntry::GetFlag(uint64_t Flag) { return (this->Value.raw & Flag) > 0 ? true : false; }

uint64_t PageDirectoryEntry::GetAddress() { return (this->Value.raw & 0x000FFFFFFFFFF000) >> 12; }

void PageDirectoryEntry::SetAddress(uint64_t Address)
{
    Address &= 0x000000FFFFFFFFFF;
    this->Value.raw &= 0xFFF0000000000FFF;
    this->Value.raw |= (Address << 12);
}
