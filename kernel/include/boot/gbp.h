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
typedef struct _GBPFramebuffer
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
} GBPFramebuffer;

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

struct GBPSMPInfo
{
    uint32_t ID;
    uint32_t LAPICID;
    uint64_t TargetStack;
    uint64_t GoToAddress;
    uint64_t ExtraArgument;
};

#define MAX_SMP 0x100

typedef struct _GBPSMP
{
    uint32_t BootstrapProcessLAPICID;
    uint64_t CPUCount;
    struct GBPSMPInfo smp[MAX_SMP];
} GBPSMP;

enum MemType
{
    GBP_Error, // 0
    GBP_Free, // 1
    GBP_Reserved, // 2
    GBP_Unusable, // 3
    GBP_ACPIReclaimable, // 4
    GBP_ACPIMemoryNVS, // 5
    GBP_Framebuffer, // 6
    GBP_Kernel, // 7
    GBP_Unknown // 8
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

#define MAX_MEMORY_ENTRIES 0x1000
#define GBP_CHECKSUM 0xDEAD5B7

struct GBPMemory
{
    uint64_t Entries;
    uint64_t Size;
    uint64_t KernelBaseVirtual;
    uint64_t KernelBasePhysical;
    struct GBPMemoryMapEntry memmap[MAX_MEMORY_ENTRIES];
};

enum initrdType
{
    InvalidRamdisk,
    USTAR,
    RAMFS
};

struct initrd
{
    enum initrdType type;
    uint64_t start;
    uint64_t end;
};

#define MAX_RAMDISKS 8

struct modules
{
    int num;
    struct initrd ramdisks[MAX_RAMDISKS];
};

struct kernel_file
{
    uint64_t file;
    uint64_t size;
};

typedef struct _GlobalBootParams
{
    char *cmdline;
    uint64_t epoch;
    struct GBPMemory mem;
    struct modules modules;
    struct kernel_file kernel;
    GBPFramebuffer *Framebuffer;
    GBPRSDP *rsdp;
    GBPSMP smp;
    uint8_t HigherHalf;
    uint32_t checksum;
} GlobalBootParams;

typedef struct _SysFlags
{
    /**
     * @brief Debug mode "debug"
     */
    bool fennecsarethebest;
    /**
     * @brief The root filesystem "rootfs=hda0 or something like that"
     */
    string rootfs; // TODO: more info about the disk
    /**
     * @brief No gpu driver
     */
    bool nogpu;
    /**
     * @brief No hpet timer
     */
    bool nohpet;
    /**
     * @brief Enter in emergency mode TODO:
     */
    bool emergency;
    /**
     * @brief Do not mount any filesystem or partition
     */
    bool nomount;
    /**
     * @brief Do not initialize the boot loading screen
     */
    bool noloadingscreen;
    // there is one more "no4gbcap" flag, but it's not used only in boot initialization code.
} SysFlags;

#endif // !__GLOBAL_BOOT_PARAMS_H__
