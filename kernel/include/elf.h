#pragma once

#include <types.h>

// https://wiki.osdev.org/ELF_Tutorial
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h

/* 32-bit ELF base types. */
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

/* 64-bit ELF base types. */
typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef int16_t Elf64_SHalf;
typedef uint64_t Elf64_Off;
typedef int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

#define EI_NIDENT 16

typedef struct elf32_hdr
{
    unsigned char e_ident[EI_NIDENT]; /* ELF "magic number" */
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry; /* Entry point virtual address */
    Elf32_Off e_phoff;  /* Program header table file offset */
    Elf32_Off e_shoff;  /* Section header table file offset */
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr
{
    unsigned char e_ident[EI_NIDENT]; /* ELF "magic number" */
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry; /* Entry point virtual address */
    Elf64_Off e_phoff;  /* Program header table file offset */
    Elf64_Off e_shoff;  /* Section header table file offset */
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct elf32_shdr
{
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct elf64_shdr
{
    Elf64_Word sh_name;       /* Section name, index in string tbl */
    Elf64_Word sh_type;       /* Type of section */
    Elf64_Xword sh_flags;     /* Miscellaneous section attributes */
    Elf64_Addr sh_addr;       /* Section virtual addr at execution */
    Elf64_Off sh_offset;      /* Section file offset */
    Elf64_Xword sh_size;      /* Size of section in bytes */
    Elf64_Word sh_link;       /* Index of another section */
    Elf64_Word sh_info;       /* Additional section information */
    Elf64_Xword sh_addralign; /* Section alignment */
    Elf64_Xword sh_entsize;   /* Entry size if section holds table */
} Elf64_Shdr;

typedef struct
{
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct elf32_rel
{
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct elf64_rel
{
    Elf64_Addr r_offset; /* Location at which to apply the action */
    Elf64_Xword r_info;  /* index and type of relocation */
} Elf64_Rel;

typedef struct elf32_sym
{
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct elf64_sym
{
    Elf64_Word st_name;     /* Symbol name, index in string tbl */
    unsigned char st_info;  /* Type and binding attributes */
    unsigned char st_other; /* No defined meaning, 0 */
    Elf64_Half st_shndx;    /* Associated section index */
    Elf64_Addr st_value;    /* Value of the symbol */
    Elf64_Xword st_size;    /* Associated symbol size */
} Elf64_Sym;

enum Elf_Ident
{
    EI_MAG0 = 0,       // 0x7F
    EI_MAG1 = 1,       // 'E'
    EI_MAG2 = 2,       // 'L'
    EI_MAG3 = 3,       // 'F'
    EI_CLASS = 4,      // Architecture (32/64)
    EI_DATA = 5,       // Byte Order
    EI_VERSION = 6,    // ELF Version
    EI_OSABI = 7,      // OS Specific
    EI_ABIVERSION = 8, // OS Specific
    EI_PAD = 9         // Padding
};

enum Elf_OSABI
{
    ELFOSABI_NONE = 0,
    ELFOSABI_HPUX = 1,
    ELFOSABI_NETBSD = 2,
    ELFOSABI_LINUX = 3,
    ELFOSABI_HURD = 4,
    ELFOSABI_SOLARIS = 6,
    ELFOSABI_AIX = 7,
    ELFOSABI_IRIX = 8,
    ELFOSABI_FREEBSD = 9,
    ELFOSABI_TRU64 = 10,
    ELFOSABI_MODESTO = 11,
    ELFOSABI_OPENBSD = 12,
    ELFOSABI_OPENVMS = 13,
    ELFOSABI_NSK = 14,
    ELFOSABI_AROS = 15,
    ELFOSABI_FENIXOS = 16, /* Wait... what? */
    ELFOSABI_CLOUDABI = 17,
    ELFOSABI_OPENVOS = 18,
    ELFOSABI_C6000_ELFABI = 64,
    ELFOSABI_C6000_LINUX = 65,
    ELFOSABI_ARM = 97,
    ELFOSABI_STANDALONE = 255
};

enum Elf_Type
{
    ET_NONE = 0,        // Unknown Type
    ET_REL = 1,         // Relocatable File
    ET_EXEC = 2,        // Executable File
    ET_DYN = 3,         // Shared Object File
    ET_CORE = 4,        // Core File
    ET_LOPROC = 0xff00, // Processor Specific
    ET_HIPROC = 0xffff  // Processor Specific
};

enum RtT_Types
{
    R_386_NONE = 0, // No relocation
    R_386_32 = 1,   // Symbol + Offset
    R_386_PC32 = 2  // Symbol + Offset - Section Offset
};

enum StT_Bindings
{
    /**
     * @brief Local symbol. Symbol is not visible outside the object file.
     */
    STB_LOCAL = 0,
    /**
     * @brief Global symbol. These symbols are visible to all object files being combined.
     */
    STB_GLOBAL = 1,
    /**
     * @brief Weak symbols. These symbols are like global symbols, but their definitions are not required. Weak symbols are not visible outside the object file containing their definition.
     */
    STB_WEAK = 2,
    /**
     * @brief Values in this inclusive range are reserved for operating system-specific semantics.
     */
    STB_LOOS = 10,
    /**
     * @brief Values in this inclusive range are reserved for operating system-specific semantics.
     */
    STB_HIOS = 12,
    /**
     * @brief Values in this inclusive range are reserved for processor-specific semantics.
     */
    STB_LOPROC = 13,
    /**
     * @brief Values in this inclusive range are reserved for processor-specific semantics.
     */
    STB_HIPROC = 15
};

enum StT_Types
{
    STT_NOTYPE = 0, // No type
    STT_OBJECT = 1, // Variables, arrays, etc.
    STT_FUNC = 2    // Methods or functions
};

enum SegmentTypes
{
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_LOSUNW = 0x6ffffffa,
    PT_SUNWBSS = 0x6ffffffb,
    PT_SUNWSTACK = 0x6ffffffa,
    PT_HISUNW = 0x6fffffff,
    PT_LOPROC = 0x70000000,
    PT_HIPROC = 0x7fffffff
};

// used for Elf64_Sym st_info
#define ELF32_ST_BIND(info) ((info) >> 4)
#define ELF32_ST_TYPE(info) ((info)&0xf)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type)&0xf))
#define ELF64_ST_BIND(info) ((info) >> 4)
#define ELF64_ST_TYPE(info) ((info)&0xf)
#define ELF64_ST_INFO(bind, type) (((bind) << 4) + ((type)&0xf))

// used for Elf64_Sym st_other
#define ELF32_ST_VISIBILITY(o) ((o)&0x3)
#define ELF64_ST_VISIBILITY(o) ((o)&0x3)

#define DO_386_32(S, A) ((S) + (A))
#define DO_386_PC32(S, A, P) ((S) + (A) - (P))

#define ELF64_R_SYM(INFO) ((INFO) >> 8)
#define ELF64_R_TYPE(INFO) ((uint8_t)(INFO))

#define SHN_UNDEF 0
#define SHN_ABS 0xfff1

#define SHT_NOBITS 8
#define SHT_REL 9

#define SHF_ALLOC 0x2

#define EM_386 (3)      // x86 Machine Type
#define EM_AMD64 (0x3E) // 64bit
#define EV_CURRENT (1)  // ELF Current Version
#define ELF_RELOC_ERR -1

#define ELFMAG0 0x7F // e_ident[EI_MAG0]
#define ELFMAG1 'E'  // e_ident[EI_MAG1]
#define ELFMAG2 'L'  // e_ident[EI_MAG2]
#define ELFMAG3 'F'  // e_ident[EI_MAG3]

#define ELFDATANONE 0 /* e_ident[EI_DATA] */
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ELFCLASSNONE 0 /* EI_CLASS */
#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFCLASSNUM 3

#define SHT_NULL 0                    /* Section header table entry unused */
#define SHT_PROGBITS 1                /* Program data */
#define SHT_SYMTAB 2                  /* Symbol table */
#define SHT_STRTAB 3                  /* String table */
#define SHT_RELA 4                    /* Relocation entries with addends */
#define SHT_HASH 5                    /* Symbol hash table */
#define SHT_DYNAMIC 6                 /* Dynamic linking information */
#define SHT_NOTE 7                    /* Notes */
#define SHT_NOBITS 8                  /* Program space with no data (bss) */
#define SHT_REL 9                     /* Relocation entries, no addends */
#define SHT_SHLIB 10                  /* Reserved */
#define SHT_DYNSYM 11                 /* Dynamic linker symbol table */
#define SHT_INIT_ARRAY 14             /* Array of constructors */
#define SHT_FINI_ARRAY 15             /* Array of destructors */
#define SHT_PREINIT_ARRAY 16          /* Array of pre-constructors */
#define SHT_GROUP 17                  /* Section group */
#define SHT_SYMTAB_SHNDX 18           /* Extended section indeces */
#define SHT_NUM 19                    /* Number of defined types.  */
#define SHT_LOOS 0x60000000           /* Start OS-specific.  */
#define SHT_GNU_ATTRIBUTES 0x6ffffff5 /* Object attributes.  */
#define SHT_GNU_HASH 0x6ffffff6       /* GNU-style hash table.  */
#define SHT_GNU_LIBLIST 0x6ffffff7    /* Prelink library list */
#define SHT_CHECKSUM 0x6ffffff8       /* Checksum for DSO content.  */
#define SHT_LOSUNW 0x6ffffffa         /* Sun-specific low bound.  */
#define SHT_SUNW_move 0x6ffffffa
#define SHT_SUNW_COMDAT 0x6ffffffb
#define SHT_SUNW_syminfo 0x6ffffffc
#define SHT_GNU_verdef 0x6ffffffd  /* Version definition section.  */
#define SHT_GNU_verneed 0x6ffffffe /* Version needs section.  */
#define SHT_GNU_versym 0x6fffffff  /* Version symbol table.  */
#define SHT_HISUNW 0x6fffffff      /* Sun-specific high bound.  */
#define SHT_HIOS 0x6fffffff        /* End OS-specific type */
#define SHT_LOPROC 0x70000000      /* Start of processor-specific */
#define SHT_HIPROC 0x7fffffff      /* End of processor-specific */
#define SHT_LOUSER 0x80000000      /* Start of application-specific */
#define SHT_HIUSER 0x8fffffff      /* End of application-specific */
