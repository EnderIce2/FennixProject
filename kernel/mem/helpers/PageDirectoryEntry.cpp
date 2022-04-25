#include <heap.h>

using namespace VMM;

void PageDirectoryEntry::AddFlag(uint64_t Flag) { this->Value |= Flag; }

void PageDirectoryEntry::RemoveFlags(uint64_t Flag) { this->Value &= ~Flag; }

void PageDirectoryEntry::ClearFlags() { this->Value = 0; }

void PageDirectoryEntry::SetFlag(uint64_t Flag, bool Enabled)
{
    this->Value &= ~Flag;
    if (Enabled)
        this->Value |= Flag;
}

uint64_t PageDirectoryEntry::GetFlag() { return this->Value; }

bool PageDirectoryEntry::GetFlag(uint64_t Flag) { return (this->Value & Flag) > 0 ? true : false; }

uint64_t PageDirectoryEntry::GetAddress() { return (this->Value & 0x000FFFFFFFFFF000) >> 12; }

void PageDirectoryEntry::SetAddress(uint64_t Address)
{
    Address &= 0x000000FFFFFFFFFF;
    this->Value &= 0xFFF0000000000FFF;
    this->Value |= (Address << 12);
}
