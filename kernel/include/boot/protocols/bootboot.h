/*
 * bootboot.h
 * https://gitlab.com/bztsrc/bootboot
 *
 * Copyright (C) 2017 - 2021 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This file is part of the BOOTBOOT Protocol package.
 * @brief The BOOTBOOT structure
 *
 */

#ifndef _BOOTBOOT_H_
#define _BOOTBOOT_H_

#ifdef  __cplusplus
extern "C" {
#endif
#ifndef _MSC_VER
#define _pack __attribute__((packed))
#else
#define _pack
#pragma pack(push)
#pragma pack(1)
#endif

#define BOOTBOOT_MAGIC "BOOT"

/* default virtual addresses for level 0 and 1 static loaders */
#define BOOTBOOT_MMIO   0xfffffffff8000000  /* memory mapped IO virtual address */
#define BOOTBOOT_FB     0xfffffffffc000000  /* frame buffer virtual address */
#define BOOTBOOT_INFO   0xffffffffffe00000  /* bootboot struct virtual address */
#define BOOTBOOT_ENV    0xffffffffffe01000  /* environment string virtual address */
#define BOOTBOOT_CORE   0xffffffffffe02000  /* core loadable segment start */

/* minimum protocol level:
 *  hardcoded kernel name, static kernel memory addresses */
#define PROTOCOL_MINIMAL 0
/* static protocol level:
 *  kernel name parsed from environment, static kernel memory addresses */
#define PROTOCOL_STATIC  1
/* dynamic protocol level:
 *  kernel name parsed, kernel memory addresses from ELF or PE symbols */
#define PROTOCOL_DYNAMIC 2
/* big-endian flag */
#define PROTOCOL_BIGENDIAN 0x80

/* loader types, just informational */
#define LOADER_BIOS     (0<<2)
#define LOADER_UEFI     (1<<2)
#define LOADER_RPI      (2<<2)
#define LOADER_COREBOOT (3<<2)

/* framebuffer pixel format, only 32 bits supported */
#define FB_ARGB   0
#define FB_RGBA   1
#define FB_ABGR   2
#define FB_BGRA   3

/* mmap entry, type is stored in least significant tetrad (half byte) of size
 * this means size described in 16 byte units (not a problem, most modern
 * firmware report memory in pages, 4096 byte units anyway). */
typedef struct {
  long unsigned int   ptr;
  long unsigned int   size;
} _pack MMapEnt;
#define MMapEnt_Ptr(a)  ((a)->ptr)
#define MMapEnt_Size(a) ((a)->size & 0xFFFFFFFFFFFFFFF0)
#define MMapEnt_Type(a) ((a)->size & 0xF)
#define MMapEnt_IsFree(a) (((a)->size&0xF)==1)

#define MMAP_USED     0   /* don't use. Reserved or unknown regions */
#define MMAP_FREE     1   /* usable memory */
#define MMAP_ACPI     2   /* acpi memory, volatile and non-volatile as well */
#define MMAP_MMIO     3   /* memory mapped IO region */

#define INITRD_MAXSIZE 16 /* Mb */

typedef struct {
  /* first 64 bytes is platform independent */
  unsigned char    magic[4];    /* 'BOOT' magic */
  unsigned int   size;        /* length of bootboot structure, minimum 128 */
  unsigned char    protocol;    /* 1, static addresses, see PROTOCOL_* and LOADER_* above */
  unsigned char    fb_type;     /* framebuffer type, see FB_* above */
  short unsigned int   numcores;    /* number of processor cores */
  short unsigned int   bspid;       /* Bootsrap processor ID (Local APIC Id on x86_64) */
  long int    timezone;    /* in minutes -1440..1440 */
  unsigned char    datetime[8]; /* in BCD yyyymmddhhiiss UTC (independent to timezone) */
  long unsigned int   initrd_ptr;  /* ramdisk image position and size */
  long unsigned int   initrd_size;
  long unsigned int   fb_ptr;      /* framebuffer pointer and dimensions */
  unsigned int   fb_size;
  unsigned int   fb_width;
  unsigned int   fb_height;
  unsigned int   fb_scanline;

  /* the rest (64 bytes) is platform specific */
  union {
    struct {
      long unsigned int acpi_ptr;
      long unsigned int smbi_ptr;
      long unsigned int efi_ptr;
      long unsigned int mp_ptr;
      long unsigned int unused0;
      long unsigned int unused1;
      long unsigned int unused2;
      long unsigned int unused3;
    } x86_64;
    struct {
      long unsigned int acpi_ptr;
      long unsigned int mmio_ptr;
      long unsigned int efi_ptr;
      long unsigned int unused0;
      long unsigned int unused1;
      long unsigned int unused2;
      long unsigned int unused3;
      long unsigned int unused4;
    } aarch64;
  } arch;

  /* from 128th byte, MMapEnt[], more records may follow */
  MMapEnt    mmap;
  /* use like this:
   * MMapEnt *mmap_ent = &bootboot.mmap; mmap_ent++;
   * until you reach bootboot->size, while(mmap_ent < bootboot + bootboot->size) */
} _pack BOOTBOOT;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#endif