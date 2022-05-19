#pragma once
#include <stdint.h>

namespace MonotonLib
{

    struct PrintPos
    {
        uint32_t x;
        uint32_t y;
    };

    class mtl
    {
    private:
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

        struct Framebuffer
        {
            uint64_t Address;
            uint32_t Width, Height;
            uint32_t PixelsPerScanLine;
        };

        uint32_t Xpos = 0, Ypos = 0;
        PSF2_FONT *PSF2Font = nullptr;
        Framebuffer FB = {0, 0, 0, 0};
        uint32_t Background = 0x000000, Foreground = 0xFFFFFF;

    public:
        mtl(char *FontPath);
        ~mtl();

        void SetPrintPosition(PrintPos Position);
        PrintPos GetPrintPosition();

        void Clear();
        void Scroll();
        void RemoveChar();
        void RemoveChar(uint32_t X, uint32_t Y);
        void printchar(char Char);
        void printchar(char Char, PrintPos Position);
        void print(const char *Text, PrintPos Position);
        void print(const char *Text);
    };
}
