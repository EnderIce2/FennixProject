#include "limine.h"
#include "../kernel.h"
#include "../drivers/serial.h"

#define PHYSICAL_TO_VIRTUAL(addr) ((uint64_t)(addr) + 0xFFFFFFFF80000000)
#define VIRTUAL_TO_PHYSICAL(addr) ((uint64_t)(addr)-0xFFFFFFFF80000000)

static volatile struct limine_entry_point_request EntryPointRequest = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0,
    .response = NULL,
    .entry = limine_initializator};

static volatile struct limine_bootloader_info_request BootloaderInfoRequest = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0};

static volatile struct limine_terminal_request TerminalRequest = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0};

static volatile struct limine_framebuffer_request FramebufferRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};
static volatile struct limine_memmap_request MemmapRequest = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0};

static volatile struct limine_kernel_address_request KernelAddressRequest = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0};

static volatile struct limine_rsdp_request RsdpRequest = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0};

static volatile struct limine_kernel_file_request KernelFileRequest = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0};

static volatile struct limine_smp_request SmpRequest = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0};

static volatile struct limine_module_request ModuleRequest = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0};

bool init_limine(GlobalBootParams *params, bool Second)
{
    params->checksum = GBP_CHECKSUM;
    if (!Second)
    {
        init_serial(COM1);
        init_serial(COM2);
        init_serial(COM3);
        init_serial(COM4);
        serial_write_text(COM1, "Initializing Limine...\n");
    }

    struct limine_bootloader_info_response *BootloaderInfoResponse = BootloaderInfoRequest.response;
    struct limine_terminal_response *TerminalResponse = TerminalRequest.response;
    struct limine_framebuffer_response *FrameBufferResponse = FramebufferRequest.response;
    struct limine_memmap_response *MemmapResponse = MemmapRequest.response;
    struct limine_kernel_address_response *KernelAddressResponse = KernelAddressRequest.response;
    struct limine_rsdp_response *RsdpResponse = RsdpRequest.response;
    struct limine_kernel_file_response *KernelFileResponse = KernelFileRequest.response;
    struct limine_smp_response *SmpResponse = SmpRequest.response;
    struct limine_module_response *ModuleResponse = ModuleRequest.response;

    if (!Second)
    {
        if (TerminalResponse == NULL || TerminalResponse->terminal_count < 1)
        {
            warn("No terminal available [%p;%ld]", TerminalResponse,
                 (TerminalResponse == NULL) ? 0 : TerminalResponse->terminal_count);
        }
        else
        {
            struct limine_terminal *terminal = TerminalResponse->terminals[0];
            TerminalResponse->write(terminal, "Please wait... ", 15);
        }
        trace("Bootloader: %s %s", BootloaderInfoResponse->name, BootloaderInfoResponse->version);
    }

    if (FrameBufferResponse == NULL || FrameBufferResponse->framebuffer_count < 1)
    {
        err("No framebuffer available [%p;%ld]", FrameBufferResponse,
            (FrameBufferResponse == NULL) ? 0 : FrameBufferResponse->framebuffer_count);
        return false;
    }
    else
    {
        params->Framebuffer.BaseAddress = (uint64_t)FrameBufferResponse->framebuffers[0]->address;
        params->Framebuffer.BufferSize = (uint64_t)(FrameBufferResponse->framebuffers[0]->pitch * FrameBufferResponse->framebuffers[0]->height);
        params->Framebuffer.Width = FrameBufferResponse->framebuffers[0]->width;
        params->Framebuffer.Height = FrameBufferResponse->framebuffers[0]->height;
        params->Framebuffer.PixelsPerScanLine = FrameBufferResponse->framebuffers[0]->pitch / 4;
        trace("Framebuffer: %016p (%d)", FrameBufferResponse->framebuffers[0]->address, FrameBufferResponse->framebuffer_count);
    }

    if (MemmapResponse == NULL || MemmapResponse->entry_count < 1)
    {
        err("No memory map available [%p;%ld]", MemmapResponse,
            (MemmapResponse == NULL) ? 0 : MemmapResponse->entry_count);
        return false;
    }
    else
    {
        uint64_t MemorySizeBytes = 0;
        int mmaptmpcount = 0;
        for (uint64_t i = 0; i < MemmapResponse->entry_count; i++)
        {
            struct limine_memmap_entry *entry = MemmapResponse->entries[i];
            uint64_t MemmapPages = (uint64_t)entry->length / 4096 + 1;
            MemorySizeBytes += entry->length;
            switch (entry->type)
            {
            case LIMINE_MEMMAP_RESERVED:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[i].Type = GBP_Reserved;
                break;
            case LIMINE_MEMMAP_USABLE:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[i].Type = GBP_Free;
                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[i].Type = GBP_Unusable;
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[i].Type = GBP_ACPIReclaimable;
                break;
            case LIMINE_MEMMAP_ACPI_NVS:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[i].Type = GBP_ACPIMemoryNVS;
                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: //? What I can do with this?
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[mmaptmpcount].Type = GBP_Unknown;
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[mmaptmpcount].Type = GBP_Kernel;
                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[mmaptmpcount].Type = GBP_Framebuffer;
                break;
            default:
                params->mem.Entry[i].PhysicalAddress = (PHYSICAL_ADDRESS)entry->base;
                params->mem.Entry[i].VirtualAddress = (VIRTUAL_ADDRESS)PHYSICAL_TO_VIRTUAL(entry->base);
                params->mem.Entry[i].Pages = MemmapPages;
                params->mem.Entry[i].Length = entry->length;
                params->mem.Entry[i].Size = MemmapPages * 4096;
                params->mem.Entry[mmaptmpcount].Type = GBP_Unknown;
                break;
            }
            mmaptmpcount++;
        }
        params->mem.Entries = mmaptmpcount;
        params->mem.Size = MemorySizeBytes;
    }

    if (KernelAddressResponse == NULL)
    {
        err("No kernel address available [%p]", KernelAddressResponse);
        return false;
    }
    else
    {
        params->mem.KernelBasePhysical = KernelAddressResponse->physical_base;
        params->mem.KernelBaseVirtual = KernelAddressResponse->virtual_base;
        trace("Kernel address: %016p", KernelAddressResponse->physical_base);
    }

    if (RsdpResponse == NULL || RsdpResponse->address == 0)
    {
        err("No RSDP address available [%p;%p]", RsdpResponse,
            (RsdpResponse == NULL) ? 0 : RsdpResponse->address);
        return false;
    }
    else
    {
        params->rsdp = (GBPRSDP *)RsdpResponse->address;
        trace("RSDP: %016p", RsdpResponse->address);
    }

    if (KernelFileResponse == NULL || KernelFileResponse->kernel_file == NULL)
    {
        err("No kernel file available [%p;%p]", KernelFileResponse,
            (KernelFileResponse == NULL) ? 0 : KernelFileResponse->kernel_file);
        return false;
    }
    else
    {
        params->cmdline = KernelFileResponse->kernel_file->cmdline;
        params->Kernel.File = (uint64_t)KernelFileResponse->kernel_file->address;
        params->Kernel.Length = KernelFileResponse->kernel_file->size;
        trace("Kernel File: %016p", KernelFileResponse->kernel_file);
    }

    if (SmpResponse == NULL || SmpResponse->cpu_count < 1)
    {
        err("No SMP information available [%p;%ld]", SmpResponse,
            (SmpResponse == NULL) ? 0 : SmpResponse->cpu_count);
        return false;
    }
    else
    {
        params->CPUCount = SmpResponse->cpu_count;
        trace("CPU Count: %d", SmpResponse->cpu_count);
    }

    if (ModuleResponse == NULL || ModuleResponse->module_count < 1)
    {
        err("No module information available [%p;%ld]", ModuleResponse,
            (ModuleResponse == NULL) ? 0 : ModuleResponse->module_count);
        return false;
    }
    else
    {
        if (ModuleResponse->module_count > 1)
            trace("There are more modules available. Picking first as initrd...");
        for (size_t i = 0; i < ModuleResponse->module_count; i++)
        {
            // TODO: drivers?
        }
        struct limine_file *module = ModuleResponse->modules[0];
        params->Modules.Module[0].Type = KernelRamdisk;
        params->Modules.Module[0].Start = (uint64_t)module->address;
        params->Modules.Module[0].Length = module->size;
        params->Modules.num++;
        trace("Module: %016p %s", module->address, module->path);
    }

    return true;
}
