#pragma once

// Comment or uncomment the following line if you want to log the heap operations
// #define DEBUG_MEM_ALLOCATION 1

#include <types.h>

extern uint64_t _kernel_start, _kernel_end;
extern uint64_t _kernel_text_end, _kernel_rodata_end;

#ifdef __cplusplus
#include <bitmap.hpp>

namespace PMM
{
    class PageFrameAllocator
    {
    public:
        bool Initalized = false;
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
        void AddFlag(uint64_t Flag);
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
        bool Initalized = false;
        PageTable *PML4;
        PageTableManager(PageTable *PML4Address);
        void MapMemory(void *VirtualAddress, void *PhysicalAddress, uint64_t Flags);
        void UnmapMemory(void *VirtualAddress);
    };
}

namespace PageTableHeap
{
    class PageTableHeap
    {
    public:
        VMM::PageTable *NewPageTable();
        void FreePageTable(VMM::PageTable *PageTable);
        PageTableHeap();
        ~PageTableHeap();
    };
}

namespace StackHeap
{
    class StackHeap
    {
    public:
        void *AllocateStack(bool User = false);
        void FreeStack(void *Address);
        StackHeap();
        ~StackHeap();
    };
}

extern PMM::PageFrameAllocator KernelAllocator;
extern VMM::PageTableManager KernelPageTableManager;
extern PageTableHeap::PageTableHeap *KernelPageTableAllocator;
extern StackHeap::StackHeap *KernelStackAllocator;

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

#define KERNEL_STACK_HEAP_BASE 0xFFFFA00000000000
#define KERNEL_STACK_HEAP_END 0xFFFFAFFFFFFF0000

#define USER_STACK_HEAP_BASE 0xFFFFB00000000000
#define USER_STACK_HEAP_END 0xFFFFBFFFFFFF0000

#define KERNEL_HEAP_BASE 0xFFFFC00000000000
#define KERNEL_HEAP_END 0xFFFFC00080000000

/**
 * @brief https://wiki.osdev.org/images/4/41/64-bit_page_tables1.png
 * @brief https://wiki.osdev.org/images/6/6b/64-bit_page_tables2.png
 */
enum PTFlag
{
    /** @brief Present */
    P = 1 << 0,

    /** @brief Read/Write */
    RW = 1 << 1,

    /** @brief User/Supervisor */
    US = 1 << 2,

    /** @brief Write-Through */
    PWT = 1 << 3,

    /** @brief Cache Disable */
    PCD = 1 << 4,

    /** @brief Accessed */
    A = 1 << 5,

    /** @brief Dirty */
    D = 1 << 6,

    /** @brief Page Size */
    PS = 1 << 7,

    /** @brief Global */
    G = 1 << 8,

    /** @brief Available 0 */
    AVL0 = 1 << 9,

    /** @brief Available 1 */
    AVL1 = 1 << 10,

    /** @brief Available 2 */
    AVL2 = 1 << 11,

    /** @brief Page Attribute Table */
    PAT = 1 << 12,

    /** @brief Available 3 */
    AVL3 = (uint64_t)1 << 52,

    /** @brief Available 4 */
    AVL4 = (uint64_t)1 << 53,

    /** @brief Available 5 */
    AVL5 = (uint64_t)1 << 54,

    /** @brief Available 6 */
    AVL6 = (uint64_t)1 << 55,

    /** @brief Available 7 */
    AVL7 = (uint64_t)1 << 56,

    /** @brief Available 8 */
    AVL8 = (uint64_t)1 << 57,

    /** @brief Available 9 */
    AVL9 = (uint64_t)1 << 58,

    /** @brief Protection Key 0 */
    PK0 = (uint64_t)1 << 59,

    /** @brief Protection Key 1 */
    PK1 = (uint64_t)1 << 60,

    /** @brief Protection Key 2 */
    PK2 = (uint64_t)1 << 61,

    /** @brief Protection Key 3 */
    PK3 = (uint64_t)1 << 62,

    /** @brief Execute Disable */
    XD = (uint64_t)1 << 63
};

enum AllocationAlgorithm
{
    Default,
    LibAlloc,
    LibAlloc11,
    BuddyAlloc
};

START_EXTERNC

void *HeapMalloc(size_t Size);
void *HeapCalloc(size_t n, size_t Size);
void *HeapRealloc(void *Address, size_t Size);
void HeapFree(void *Address);

#ifndef DEBUG_MEM_ALLOCATION
#define kmalloc(Size) HeapMalloc(Size)
#define kcalloc(n, Size) HeapCalloc(n, Size)
#define krealloc(Address, Size) HeapRealloc(Address, Size)
#define kfree(Address) HeapFree(Address)
#else
void *dbg_malloc(size_t Size, string file, int line, string function);
void dbg_free(void *Address, string file, int line, string function);
void *dbg_calloc(size_t n, size_t Size, string file, int line, string function);
void *dbg_realloc(void *Address, size_t Size, string file, int line, string function);

#define kmalloc(Size) dbg_malloc(Size, __FILE__, __LINE__, __FUNCTION__)
#define kcalloc(n, Size) dbg_calloc(n, Size, __FILE__, __LINE__, __FUNCTION__)
#define krealloc(Address, Size) dbg_realloc(Address, Size, __FILE__, __LINE__, __FUNCTION__)
#define kfree(Address) dbg_free(Address, __FILE__, __LINE__, __FUNCTION__)

#endif

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
void init_kernelpml();
void init_heap(enum AllocationAlgorithm Type);

END_EXTERNC
