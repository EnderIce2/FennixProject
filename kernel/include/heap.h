#pragma once

#include <types.h>

extern uint64_t _kernel_start, _kernel_end;
extern uint64_t _kernel_text_end, _kernel_rodata_end;

#ifdef __cplusplus
#include <bitmap.h>

namespace PMM
{
    class PageFrameAllocator
    {
    public:
        void ReadMemoryMap();
        Bitmap PageBitmap;
        void FreePage(void *Address);
        void FreePages(void *Address, uint64_t PageCount);
        void LockPage(void *Address);
        void LockPages(void *Address, uint64_t PageCount);
        void *RequestPage();
        void *RequestPages(uint64_t PageCount);
        uint64_t GetFreeRAM();
        uint64_t GetUsedRAM();
        uint64_t GetReservedRAM();

    private:
        void InitBitmap(size_t BitmapSize, void *BufferAddress);
        void ReservePage(void *Address);
        void ReservePages(void *Address, uint64_t PageCount);
        void UnreservePage(void *Address);
        void UnreservePages(void *Address, uint64_t PageCount);
    };
}

namespace VMM
{
    class PageMapIndexer
    {
    public:
        PageMapIndexer(uint64_t VirtualAddress);
        uint64_t PDP_i;
        uint64_t PD_i;
        uint64_t PT_i;
        uint64_t P_i;
    };

    struct PageDirectoryEntry
    {
        uint64_t Value;
        void AddFlags(uint64_t Flag);
        void RemoveFlags(uint64_t Flag);
        void ClearFlags();
        void SetFlag(uint64_t Flag, bool Enabled);
        bool GetFlag(uint64_t Flag);
        uint64_t GetFlag();
        void SetAddress(uint64_t Address);
        uint64_t GetAddress();
    };

    struct PageTable
    {
        PageDirectoryEntry Entries[512];
    } __attribute__((aligned(0x1000)));

    class PageTableManager
    {
    public:
        PageTable *PML4;
        PageTableManager(PageTable *PML4Address);
        void MapMemory(void *VirtualAddress, void *PhysicalAddress, uint64_t Flags);
        void UnmapMemory(void *VirtualAddress);
    };
}

namespace Heap
{
    struct HeapSegHdr
    {
        size_t Length;
        HeapSegHdr *Next;
        HeapSegHdr *Last;
        bool IsFree;
        void CombineForward();
        void CombineBackward();
        HeapSegHdr *Split(size_t SplitLength);
    };
}

extern PMM::PageFrameAllocator KernelAllocator;
extern VMM::PageTableManager KernelPageTableManager;
void ExpandHeap(size_t Length);

void *operator new(size_t Size);
void *operator new[](size_t Size);
void operator delete(void *Pointer);
void operator delete[](void *Pointer);
void operator delete(void *Pointer, long unsigned int n);
void operator delete[](void *Pointer, long unsigned int n);

#endif

#define STACK_SIZE 0x1000
#define PAGE_SIZE 0x1000

#define NORMAL_VMA_OFFSET 0xFFFF800000000000
#define KERNEL_VMA_OFFSET 0xFFFFFFFF80000000

// 2GB
#define KERNEL_HEAP_BASE 0xFFFFC00000000000
#define KERNEL_HEAP_END 0xFFFFC00080000000

// 2GB
#define KERNEL_STACK_HEAP_BASE 0xFFFFE00000000000
#define KERNEL_STACK_HEAP_END 0xFFFFE00080000000

/**
 * @brief https://wiki.osdev.org/images/4/41/64-bit_page_tables1.png
 * @brief https://wiki.osdev.org/images/6/6b/64-bit_page_tables2.png
 */
enum PTFlag
{
    /** @brief Present */
    P    = 0x00000000,

    /** @brief Read/Write */
    RW   = 0x00000001,

    /** @brief User/Supervisor */
    US   = 0x00000002,

    /** @brief Write-Through */
    PWT  = 0x00000003,

    /** @brief Cache Disable */
    PCD  = 0x00000004,

    /** @brief Accessed */
    A    = 0x00000005,

    /** @brief Dirty */
    D    = 0x00000006,

    /** @brief Page Size */
    PS   = 0x00000007,

    /** @brief Global */
    G    = 0x00000008,

    /** @brief Available 0 */
    AVL0 = 0x00000009,

    /** @brief Available 1 */
    AVL1 = 0x00000010,

    /** @brief Available 2 */
    AVL2 = 0x00000011,

    /** @brief Page Attribute Table */
    PAT  = 0x00000012,

    /** @brief Available 3 */
    AVL3 = 0x00000052,

    /** @brief Available 4 */
    AVL4 = 0x00000053,

    /** @brief Available 5 */
    AVL5 = 0x00000054,

    /** @brief Available 6 */
    AVL6 = 0x00000055,

    /** @brief Available 7 */
    AVL7 = 0x00000056,

    /** @brief Available 8 */
    AVL8 = 0x00000057,

    /** @brief Available 9 */
    AVL9 = 0x00000058,

    /** @brief Protection Key 0 */
    PK0  = 0x00000059,

    /** @brief Protection Key 1 */
    PK1  = 0x00000060,

    /** @brief Protection Key 2 */
    PK2  = 0x00000061,

    /** @brief Protection Key 3 */
    PK3  = 0x00000062,

    /** @brief Execute Disable */
    XD   = 0x00000063
};

START_EXTERNC

void *AllocateStack();
void FreeStack(void *stack);
void init_stack();

void *kmalloc(size_t Size);
void *kcalloc(size_t n, size_t Size);
void *krealloc(void *Address, size_t Size);
void kfree(void *Address);
void *RequestPage();
void *RequestPages(uint64_t pages);
#ifdef __cplusplus
VMM::PageTable *CreateNewPML4();
#else
void *CreateNewPML4();
#endif
void FreePage(void *address);
void FreePages(void *address, uint64_t pages);
uint64_t GetFreeMemory();
uint64_t GetUsedMemory();
uint64_t GetReservedMemory();
void MapMemory(void *PML4, void *VirtualMemory, void *PhysicalMemory, uint64_t Flags);

void init_pmm();
void init_vmm();
void init_heap(void *HeapAddress, size_t PageCount);

END_EXTERNC