#pragma once

#include <stdint.h>

typedef unsigned short WORD;
typedef wchar_t WCHAR;
typedef unsigned short USHORT;
typedef long LONG;

#define IMAGE_DOS_SIGNATURE 0x5A4D      /* MZ */
#define IMAGE_OS2_SIGNATURE 0x454E      /* NE */
#define IMAGE_OS2_SIGNATURE_LE 0x454C   /* LE */
#define IMAGE_NT_SIGNATURE 0x00004550   /* PE00 */
#define IMAGE_EDOS_SIGNATURE 0x44454550 /* PEED */

#define IMAGE_SIZEOF_FILE_HEADER 20

#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I860 0x14d
#define IMAGE_FILE_MACHINE_I386 0x14c
#define IMAGE_FILE_MACHINE_R3000 0x162
#define IMAGE_FILE_MACHINE_R4000 0x166
#define IMAGE_FILE_MACHINE_R10000 0x0168
#define IMAGE_FILE_MACHINE_ALPHA 0x184
#define IMAGE_FILE_MACHINE_POWERPC 0x01F0
#define IMAGE_FILE_MACHINE_POWERPCBE 0x01F2
#define IMAGE_FILE_MACHINE_SH3 0x01a2
#define IMAGE_FILE_MACHINE_SH3E 0x01a4
#define IMAGE_FILE_MACHINE_SH4 0x01a6
#define IMAGE_FILE_MACHINE_ARM 0x01c0
#define IMAGE_FILE_MACHINE_THUMB 0x01c2
#define IMAGE_FILE_MACHINE_IA64 0x0200
#define IMAGE_FILE_MACHINE_MIPS16 0x0266
#define IMAGE_FILE_MACHINE_MIPSFPU 0x0366
#define IMAGE_FILE_MACHINE_MIPSFPU16 0x0466
#define IMAGE_FILE_MACHINE_ALPHA64 0x0284
#define IMAGE_FILE_MACHINE_AXP64 IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_AMD64 0x8664
#define IMAGE_FILE_MACHINE_CEF 0xC0EF

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION 3
#define IMAGE_DIRECTORY_ENTRY_SECURITY 4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5
#define IMAGE_DIRECTORY_ENTRY_DEBUG 6
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE 7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 8
#define IMAGE_DIRECTORY_ENTRY_TLS 9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 11
#define IMAGE_DIRECTORY_ENTRY_IAT 12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14

typedef struct _IMAGE_DOS_HEADER
{
    WORD e_magic;
    WORD e_cblp;
    WORD e_cp;
    WORD e_crlc;
    WORD e_cparhdr;
    WORD e_minalloc;
    WORD e_maxalloc;
    WORD e_ss;
    WORD e_sp;
    WORD e_csum;
    WORD e_ip;
    WORD e_cs;
    WORD e_lfarlc;
    WORD e_ovno;
    WORD e_res[4];
    WORD e_oemid;
    WORD e_oeminfo;
    WORD e_res2[10];
    WORD e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_OS2_HEADER
{
    uint16_t ne_magic;
    uint8_t ne_ver;
    uint8_t ne_rev;
    uint16_t ne_enttab;
    uint16_t ne_cbenttab;
    uint32_t ne_crc;
    uint16_t ne_flags;
    uint16_t ne_autodata;
    uint16_t ne_heap;
    uint16_t ne_stack;
    uint32_t ne_csip;
    uint32_t ne_sssp;
    uint16_t ne_cseg;
    uint16_t ne_cmod;
    uint16_t ne_cbnrestab;
    uint16_t ne_segtab;
    uint16_t ne_rsrctab;
    uint16_t ne_restab;
    uint16_t ne_modtab;
    uint16_t ne_imptab;
    uint32_t ne_nrestab;
    uint16_t ne_cmovent;
    uint16_t ne_align;
    uint16_t ne_cres;
    uint8_t ne_exetyp;
    uint8_t ne_flagsothers;
    uint16_t ne_pretthunks;
    uint16_t ne_psegrefbytes;
    uint16_t ne_swaparea;
    uint16_t ne_expver;
} IMAGE_OS2_HEADER, *PIMAGE_OS2_HEADER;

typedef struct _IMAGE_SECTION_HEADER
{
#define IMAGE_SIZEOF_SHORT_NAME 8
    uint8_t Name[IMAGE_SIZEOF_SHORT_NAME];
    union
    {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;

} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_FILE_HEADER
{
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY
{
    uint32_t VirtualAddress;
    uint32_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER
{
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint32_t BaseOfData;
    uint32_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS
{
    uint32_t Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER OptionalHeader;

} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;
