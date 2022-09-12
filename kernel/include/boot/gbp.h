#ifndef __GLOBAL_BOOT_PARAMS_H__
#define __GLOBAL_BOOT_PARAMS_H__

#include <types.h>

/*
 * Global Boot Parameters.
 */

/**
 * @brief Physical Address
 */
typedef uint64_t PHYSICAL_ADDRESS;

/**
 * @brief Virtual Address
 */
typedef uint64_t VIRTUAL_ADDRESS;

/**
 * @brief Framebuffer structure
 */
struct GBPFramebuffer
{
    /**
     * @brief Base address of the framebuffer
     */
    uint64_t BaseAddress;
    /**
     * @brief Framebuffer buffer size
     */
    uint64_t BufferSize;
    /**
     * @brief Screen width
     */
    uint32_t Width;
    /**
     * @brief Screen height
     */
    uint32_t Height;
    /**
     * @brief Pixels per scan line
     */
    uint32_t PixelsPerScanLine;
};

/**
 * @brief Root System Description Pointer structure
 */
typedef struct _GBPRSDP
{
    /**
     * @brief Signature
     */
    unsigned char Signature[8];
    /**
     * @brief Checksum
     */
    uint8_t Checksum;
    /**
     * @brief OEM ID
     */
    uint8_t OEMId[6];
    /**
     * @brief Revision
     */
    uint8_t Revision;
    /**
     * @brief Address of the Root System Description Table
     */
    uint32_t RSDTAddress;
    /**
     * @brief Length
     */
    uint32_t Length;
    /**
     * @brief Extended System Descriptor Table
     */
    uint64_t XSDTAddress;
    /**
     * @brief Extended checksum
     */
    uint8_t ExtendedChecksum;
    /**
     * @brief Reserved
     */
    uint8_t Reserved[3];
} __attribute__((packed)) GBPRSDP;

enum MemType
{
    GBP_Error,           // 0
    GBP_Free,            // 1
    GBP_Reserved,        // 2
    GBP_Unusable,        // 3
    GBP_ACPIReclaimable, // 4
    GBP_ACPIMemoryNVS,   // 5
    GBP_Framebuffer,     // 6
    GBP_Kernel,          // 7
    GBP_Unknown          // 8
};

struct GBPMemoryMapEntry
{
    PHYSICAL_ADDRESS PhysicalAddress;
    VIRTUAL_ADDRESS VirtualAddress;
    uint64_t Pages;
    enum MemType Type;
    uint64_t Size;
    uint64_t Length;
};

#define MAX_MEMORY_ENTRIES 0x10000
#define GBP_CHECKSUM 0xDEAD5B7

struct GBPMemory
{
    uint64_t Entries;
    uint64_t Size;
    uint64_t KernelBaseVirtual;
    uint64_t KernelBasePhysical;
    struct GBPMemoryMapEntry Entry[MAX_MEMORY_ENTRIES];
};

enum ModuleType
{
    InvalidModuleType,
    KernelRamdisk,
    KernelDriver
};

#define MAX_MODULES 16

typedef struct _GlobalBootParams
{
    char *cmdline;
    GBPRSDP *rsdp;
    struct GBPMemory mem;
    struct KernelModules
    {
        struct KernelModule
        {
            enum ModuleType Type;
            uint64_t Start;
            uint64_t Length;
        } Module[MAX_MODULES];
        int num;
    } Modules;
    struct KernelFile
    {
        uint64_t File;
        uint64_t Length;
    } Kernel;
    struct GBPFramebuffer Framebuffer;
    uint64_t CPUCount;
    uint32_t checksum;
} GlobalBootParams;

#endif // !__GLOBAL_BOOT_PARAMS_H__
