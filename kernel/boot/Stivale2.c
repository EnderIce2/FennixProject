#include "stivale2.h"
#include "../kernel.h"
#include <heap.h>
#include <string.h>
#include <asm.h>
#include "../drivers/serial.h"

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .flags = 0,
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0}};

static struct stivale2_header_tag_smp smp_hdr_tag = {
    .flags = 0,
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = (uint64_t)(&terminal_hdr_tag)}};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&smp_hdr_tag}};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = (uint64_t)stivale2_initializator,
    .stack = (uint64_t)kernel_stack + sizeof(kernel_stack),
    // (1 << 1)
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want since this is a higher half kernel.

    // (1 << 2)
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the executable's
    // segments.

    // (1 << 3)
    // Bit 3, if set, enables fully virtual kernel mappings, which we want as
    // they allow the bootloader to pick whichever *physical* memory address is
    // available to load the kernel, rather than relying on us telling it where
    // to load it.

    // (1 << 4)
    // Bit 4 disables a deprecated feature and should always be set.
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uint64_t)&framebuffer_hdr_tag};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    if ((void *)stivale2_struct->tags == NULL)
        return NULL;
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;)
    {
        if (current_tag == NULL)
        {
            return NULL;
        }
        if (current_tag->identifier == id)
        {
            return current_tag;
        }
        current_tag = (void *)current_tag->next;
    }
}

#define Stivale2_STRUCT(name, tag)                                                         \
    struct stivale2_struct_tag_##name *tag_##name = stivale2_get_tag(bootloaderdata, tag); \
    if (tag_##name == NULL)                                                                \
    {                                                                                      \
        warn("Missing tag %s!!!", #tag);                                                   \
    }

#define PHYSICAL_TO_VIRTUAL(addr) ((uint64_t)(addr) + 0xFFFFFFFF80000000)
#define VIRTUAL_TO_PHYSICAL(addr) ((uint64_t)(addr)-0xFFFFFFFF80000000)

bool init_stivale2(struct stivale2_struct *bootloaderdata, GlobalBootParams *params, bool second)
{
    params->checksum = GBP_CHECKSUM;
    uint64_t MemorySizeBytes = 0;
    int mmaptmpcount = 0;

    if (!second)
    {
        init_serial(COM1);
        init_serial(COM2);
        init_serial(COM3);
        init_serial(COM4);
        serial_write_text(COM1, "Initializing Stivale2...\n");
    }
    Stivale2_STRUCT(terminal, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    Stivale2_STRUCT(framebuffer, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    Stivale2_STRUCT(memmap, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    Stivale2_STRUCT(base_address, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    Stivale2_STRUCT(rsdp, STIVALE2_STRUCT_TAG_RSDP_ID);
    Stivale2_STRUCT(epoch, STIVALE2_STRUCT_TAG_EPOCH_ID);
    Stivale2_STRUCT(cmdline, STIVALE2_STRUCT_TAG_CMDLINE_ID);
    Stivale2_STRUCT(firmware, STIVALE2_STRUCT_TAG_FIRMWARE_ID);
    Stivale2_STRUCT(smp, STIVALE2_STRUCT_TAG_SMP_ID);
    Stivale2_STRUCT(modules, STIVALE2_STRUCT_TAG_MODULES_ID);
    Stivale2_STRUCT(kernel, STIVALE2_STRUCT_TAG_KERNEL_FILE_V2_ID);
    void (*term_write)(const char *string, size_t length) = (void *)tag_terminal->term_write;
    struct stivale2_struct_tag_cmdline *cmdline = (struct stivale2_struct_tag_cmdline *)tag_cmdline;
    if (!second)
    {
        trace("Bootloader: %s %s", bootloaderdata->bootloader_brand, bootloaderdata->bootloader_version);
        if (strstr((string)cmdline->cmdline, "no4gbcap"))
        {
            debug("Ignoring 4GB memory cap...");
        }
        else
        {
            struct stivale2_struct_tag_memmap *memmap = (struct stivale2_struct_tag_memmap *)tag_memmap;
            uint64_t tmpmemsize = 0;
            for (uint64_t i = 0; i < memmap->entries; i++)
                tmpmemsize += memmap->memmap[i].length;

            if (TO_GB(tmpmemsize) < 4)
            {
                // The system can run with less than 4GB memory but I didn't test that enough.
                term_write("Minimum memory requirement is 4GB! System Halted.", 42);
                while (1)
                    __asm__ __volatile__("hlt" ::
                                             : "memory");
            }
        }
        term_write("Please wait... ", 15);
    }

    // TODO: we have framebuffer and basic info here.
    /*
     * I can do a simple loading screen until I remap the memory because the bootloader
     * already has the framebuffer mapped.
     */

    params->HigherHalf = true;
    struct stivale2_struct_tag_framebuffer *fb = (struct stivale2_struct_tag_framebuffer *)tag_framebuffer;
    params->Framebuffer.BaseAddress = fb->framebuffer_addr;
    params->Framebuffer.BufferSize = (uint64_t)fb->framebuffer_pitch * fb->framebuffer_height;
    params->Framebuffer.Width = fb->framebuffer_width;
    params->Framebuffer.Height = fb->framebuffer_height;
    params->Framebuffer.PixelsPerScanLine = fb->framebuffer_pitch / 4;
    debug("FRAMEBUFFER: %016p", fb->framebuffer_addr);

    struct stivale2_struct_tag_rsdp *rsdp = (struct stivale2_struct_tag_rsdp *)tag_rsdp;
    params->rsdp = (GBPRSDP *)rsdp->rsdp;
    debug("RSDP: %016p", rsdp->rsdp);

    struct stivale2_struct_tag_epoch *epoch = (struct stivale2_struct_tag_epoch *)tag_epoch;
    params->epoch = epoch->epoch;
    debug("EPOCH: %010d", epoch->epoch);

    params->cmdline = (char *)cmdline->cmdline;
    debug("CMDLINE: %s", cmdline->cmdline);

    struct stivale2_struct_tag_firmware *firmware = (struct stivale2_struct_tag_firmware *)tag_firmware;
    if (firmware->flags == STIVALE2_FIRMWARE_BIOS)
    {
        debug("FIRMWARE: BIOS");
    }
    else
    {
        debug("FIRMWARE: %p", firmware->flags);
    }

    struct stivale2_struct_tag_smp *smp = (struct stivale2_struct_tag_smp *)tag_smp;
    params->smp.CPUCount = smp->cpu_count;
    params->smp.BootstrapProcessLAPICID = smp->bsp_lapic_id;
    for (size_t i = 0; i < smp->cpu_count; i++)
    {
        params->smp.smp[i].ID = smp->smp_info[i].processor_id;
        params->smp.smp[i].LAPICID = smp->smp_info[i].lapic_id;
        params->smp.smp[i].TargetStack = smp->smp_info[i].target_stack;
        params->smp.smp[i].GoToAddress = smp->smp_info[i].goto_address;
        params->smp.smp[i].ExtraArgument = smp->smp_info[i].extra_argument;
    }
    debug("CPU Count: %d", smp->cpu_count);

    struct stivale2_struct_tag_memmap *memmap = (struct stivale2_struct_tag_memmap *)tag_memmap;
    for (uint64_t i = 0; i < memmap->entries; i++)
    {
        uint64_t MemmapPages = (uint64_t)memmap->memmap[i].length / 4096 + 1;
        MemorySizeBytes += memmap->memmap[i].length;
        switch (memmap->memmap[i].type)
        {
        case STIVALE2_MMAP_RESERVED:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[i].Type = GBP_Reserved;
            break;
        case STIVALE2_MMAP_USABLE:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[i].Type = GBP_Free;
            break;
        case STIVALE2_MMAP_BAD_MEMORY:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[i].Type = GBP_Unusable;
            break;
        case STIVALE2_MMAP_ACPI_RECLAIMABLE:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[i].Type = GBP_ACPIReclaimable;
            break;
        case STIVALE2_MMAP_ACPI_NVS:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[i].Type = GBP_ACPIMemoryNVS;
            break;
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: //? What I can do with this?
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[mmaptmpcount].Type = GBP_Unknown;
            break;
        case STIVALE2_MMAP_KERNEL_AND_MODULES:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[mmaptmpcount].Type = GBP_Kernel;
            break;
        case STIVALE2_MMAP_FRAMEBUFFER:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[mmaptmpcount].Type = GBP_Framebuffer;
            break;
        default:
            params->mem.memmap[i].PhysicalAddress = (PHYSICAL_ADDRESS)memmap->memmap[i].base;
            params->mem.memmap[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(memmap->memmap[i].base);
            params->mem.memmap[i].Pages = MemmapPages;
            params->mem.memmap[i].Length = memmap->memmap[i].length;
            params->mem.memmap[i].Size = MemmapPages * 4096;
            params->mem.memmap[mmaptmpcount].Type = GBP_Unknown;
            break;
        }
        mmaptmpcount++;
    }
    params->mem.Entries = mmaptmpcount;
    params->mem.Size = MemorySizeBytes;

    struct stivale2_struct_tag_kernel_base_address *base_address = (struct stivale2_struct_tag_kernel_base_address *)tag_base_address;
    params->mem.KernelBasePhysical = base_address->physical_base_address;
    params->mem.KernelBaseVirtual = base_address->virtual_base_address;
    debug("KernelBasePhysical %016p and KernelBaseVirtual %016p",
          params->mem.KernelBasePhysical, params->mem.KernelBaseVirtual);

    for (uint64_t i = 0; i < tag_modules->module_count; i++)
    {
        if (!strcmp(tag_modules->modules[i].string, "ustar"))
            params->modules.ramdisks[i].type = USTAR;
        else if (!strcmp(tag_modules->modules[i].string, "ramfs"))
            params->modules.ramdisks[i].type = RAMFS;
        params->modules.ramdisks[i].start = tag_modules->modules[i].begin;
        params->modules.ramdisks[i].end = tag_modules->modules[i].end;
        params->modules.num++;
        debug("INITRD found %016p-%016p (%s)",
              tag_modules->modules[i].begin,
              tag_modules->modules[i].end,
              tag_modules->modules[i].string);
    }

    struct stivale2_struct_tag_kernel_file_v2 *kernel = (struct stivale2_struct_tag_kernel_file_v2 *)tag_kernel;
    params->kernel.file = kernel->kernel_file;
    params->kernel.size = kernel->kernel_size;

    if (rsdp->rsdp != (uint64_t)params->rsdp)
    {
        err("RSDP mismatch. %016p != %016p",
            rsdp->rsdp,
            params->rsdp);
        term_write("RSDP mismatch.", 14);
        while (1)
            __asm__ __volatile__("hlt" ::
                                     : "memory");
    }
    if (tag_memmap->entries != params->mem.Entries)
    {
        warn("Memory map entries mismatch. %lld != %lld",
             (struct stivale2_struct_tag_memmap *)tag_memmap->entries,
             params->mem.Entries);
        // term_write("Memory map entries mismatch.", 28);
        // while (1)
        //     __asm__ __volatile__("hlt" ::
        //                              : "memory");
    }
    if (fb->framebuffer_addr != params->Framebuffer.BaseAddress)
    {
        err("Framebuffer base address mismatch. %016p != %016p",
            fb->framebuffer_addr,
            params->Framebuffer.BaseAddress);
        term_write("Framebuffer base address mismatch.", 34);
        while (1)
            __asm__ __volatile__("hlt" ::
                                     : "memory");
    }
    debug("Stivale2 initialized");
    return true;
}

bool detect_stivale2(void *data)
{
    struct stivale2_struct *bootloaderdata = (struct stivale2_struct *)data;
    Stivale2_STRUCT(terminal, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    if (tag_terminal == NULL)
        return false;
    else
        return true;
}
