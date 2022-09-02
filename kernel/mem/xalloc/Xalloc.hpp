#pragma once
#include <heap.h>

// Functions defines

// Page allocation functions
#define Xalloc_REQUEST_PAGE() KernelAllocator.RequestPage()
#define Xalloc_REQUEST_PAGES(Pages) KernelAllocator.RequestPages(Pages)
#define Xalloc_FREE_PAGE(Address) KernelAllocator.FreePage(Address)
#define Xalloc_FREE_PAGES(Address, Pages) KernelAllocator.FreePages(Address, Pages)

#define Xalloc_MAP_MEMORY(Virtual, Physical, Flags) KernelPageTableManager.MapMemory(Virtual, Physical, Flags)
#define Xalloc_UNMAP_MEMORY(Virtual) KernelPageTableManager.UnmapMemory(Virtual)
#define Xalloc_MAP_MEMORY_READ_WRITE PTFlag::RW
#define Xalloc_MAP_MEMORY_USER PTFlag::US

#define Xalloc_PAGE_SIZE PAGE_SIZE

#define Xalloc_trace(m, ...) trace(m, ##__VA_ARGS__)
#define Xalloc_warn(m, ...) warn(m, ##__VA_ARGS__)
#define Xalloc_err(m, ...) err(m, ##__VA_ARGS__)

typedef long unsigned int Xuint64_t;

namespace Xalloc
{
    class AllocatorV1
    {
    private:
        struct HeapSegment
        {
            Xuint64_t Length;
            HeapSegment *Next;
            HeapSegment *Last;
            bool IsFree;

            HeapSegment *Split(Xuint64_t SplitLength, HeapSegment *LastSegment)
            {
                if (SplitLength < 0x10)
                    return nullptr;
                int64_t SplitSegmentLength = Length - SplitLength - (sizeof(HeapSegment));
                if (SplitSegmentLength < 0x10)
                    return nullptr;
                HeapSegment *NewSplitHdr = (HeapSegment *)((Xuint64_t)this + SplitLength + sizeof(HeapSegment));
                Next->Last = NewSplitHdr;
                NewSplitHdr->Next = Next;
                Next = NewSplitHdr;
                NewSplitHdr->Last = this;
                NewSplitHdr->Length = SplitSegmentLength;
                NewSplitHdr->IsFree = IsFree;
                Length = SplitLength;
                if (LastSegment == this)
                    LastSegment = NewSplitHdr;
                return NewSplitHdr;
            }

            void CombineForward(HeapSegment *LastSegment)
            {
                if (Next == nullptr)
                    return;
                if (Next->IsFree == false)
                    return;
                if (Next == LastSegment)
                    LastSegment = this;
                if (Next->Next != nullptr)
                    Next->Next->Last = this;

                Length = Length + Next->Length + sizeof(HeapSegment);
                Next = Next->Next;
            }

            void CombineBackward(HeapSegment *LastSegment)
            {
                if (Last != nullptr && Last->IsFree)
                    Last->CombineForward(LastSegment);
            }
        } __attribute__((aligned(16)));

        void *HeapStart = nullptr;
        void *HeapEnd = nullptr;
        HeapSegment *LastSegment = nullptr;
        bool UserMapping = false;
        bool SMAPUsed = false;

        void ExpandHeap(Xuint64_t Length);

        static inline void *Xmemcpy(void *__restrict__ Destination, const void *__restrict__ Source, Xuint64_t Length)
        {
            unsigned char *dst = (unsigned char *)Destination;
            const unsigned char *src = (const unsigned char *)Source;
            for (Xuint64_t i = 0; i < Length; i++)
                dst[i] = src[i];
            return Destination;
        }

        static inline void *Xmemset(void *__restrict__ Destination, int Data, Xuint64_t Length)
        {
            unsigned char *Buffer = (unsigned char *)Destination;
            for (Xuint64_t i = 0; i < Length; i++)
                Buffer[i] = (unsigned char)Data;
            return Destination;
        }

    public:
        inline void Xstac()
        {
            if (this->SMAPUsed)
                asm volatile("stac" ::
                                 : "cc");
        }

        inline void Xclac()
        {
            if (this->SMAPUsed)
                asm volatile("clac" ::
                                 : "cc");
        }

        /**
         * @brief Construct a new Allocator V1 object
         *
         * @param Address Virtual address to allocate.
         * @param UserMode Map the new pages with USER flag?
         * @param SMAPEnabled Does the kernel has Supervisor Mode Access Prevention enabled?
         */
        AllocatorV1(void *Address, bool UserMode, bool SMAPEnabled);

        /**
         * @brief Destroy the Allocator V 1 object
         *
         */
        ~AllocatorV1();

        /**
         * @brief Allocate a new memory block
         *
         * @param Size Size of the block to allocate.
         * @return void* Pointer to the allocated block.
         */
        void *Malloc(Xuint64_t Size);

        /**
         * @brief Free a previously allocated block
         *
         * @param Address Address of the block to free.
         */
        void Free(void *Address);

        /**
         * @brief Allocate a new memory block
         *
         * @param NumberOfBlocks Number of blocks to allocate.
         * @param Size Size of the block to allocate.
         * @return void* Pointer to the allocated block.
         */
        void *Calloc(Xuint64_t NumberOfBlocks, Xuint64_t Size);

        /**
         * @brief Reallocate a previously allocated block
         *
         * @param Address Address of the block to reallocate.
         * @param Size New size of the block.
         * @return void* Pointer to the reallocated block.
         */
        void *Realloc(void *Address, Xuint64_t Size);
    };
}
