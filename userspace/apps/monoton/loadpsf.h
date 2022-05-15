#pragma once
#include <stdint.h>

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

struct PSF2_HEADER
{
    uint8_t magic[4];
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t length;
    uint32_t charsize;
    uint32_t height, width;
};

typedef struct _PSF2_FONT
{
    PSF2_HEADER *Header;
    void *GlyphBuffer;
} PSF2_FONT;

void LoadFont(const char *filename);
