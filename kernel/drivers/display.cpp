#include <display.h>
#include <stdarg.h>
#include <printf.h>
#include <heap.h>
#include <limits.h>
#include <debug.h>

#include "../kernel.h"

extern uint64_t _binary_files_zap_ext_vga16_psf_start;
extern uint64_t _binary_files_zap_ext_vga16_psf_end;
extern uint64_t _binary_files_zap_ext_vga16_psf_size;

extern uint64_t _binary_files_zap_ext_light20_psf_start;
extern uint64_t _binary_files_zap_ext_light20_psf_end;
extern uint64_t _binary_files_zap_ext_light20_psf_size;

extern uint64_t _binary_files_zap_ext_light24_psf_start;
extern uint64_t _binary_files_zap_ext_light24_psf_end;
extern uint64_t _binary_files_zap_ext_light24_psf_size;

extern uint64_t _binary_files_ter_powerline_v12n_psf_start;
extern uint64_t _binary_files_ter_powerline_v12n_psf_end;
extern uint64_t _binary_files_ter_powerline_v12n_psf_size;

DisplayDriver::Display *CurrentDisplay = nullptr;

namespace DisplayDriver
{
    Font::Font(uint64_t *Start, uint64_t *End, FontType Type)
    {
        trace("Initializing font with start %#llx and end %#llx Type: %d", Start, End, Type);
        PSFFile.start = Start;
        PSFFile.end = End;
        type = Type;
        if (Type == FontType::PCScreenFont2)
        {
            //! TODO: Well I get a lot of messages from ubsan about this:
            /* Member access within address 0xffffffff800..... with insufficient
               space for object of type 'struct PSF2_HEADER' */
            PSF2Font = new PSF2_FONT;
            uint16_t glyph2 = 0;

            uint64_t FontDataLength = End - Start;
            PSF2_HEADER *font2 = (PSF2_HEADER *)KernelAllocator.RequestPages(FontDataLength / 4096 + 1);
            for (uint64_t i = 0; i < FontDataLength / 4096 + 1; i++)
                KernelPageTableManager.MapMemory((void *)(font2 + (i * PAGE_SIZE)), (void *)(font2 + (i * PAGE_SIZE)), PTFlag::RW);
            memcpy((void *)font2, Start, FontDataLength);

            width = font2->width;
            height = font2->height;
            if (font2->magic[0] != PSF2_MAGIC0 || font2->magic[1] != PSF2_MAGIC1 || font2->magic[2] != PSF2_MAGIC2 || font2->magic[3] != PSF2_MAGIC3)
                err("Font2 magic mismatch.");

            PSF2Font->Header = font2;
            PSF2Font->GlyphBuffer = (Start + sizeof(PSF2_HEADER));
            KernelAllocator.FreePages(font2, FontDataLength / 4096 + 1);
        }
        else if (Type == FontType::PCScreenFont1)
        {
            PSF1Font = new PSF1_FONT;
            PSF1_HEADER *font1 = (PSF1_HEADER *)Start;
            if (font1->magic[0] != PSF1_MAGIC0 || font1->magic[1] != PSF1_MAGIC1)
                err("Font1 magic mismatch.");
            uint32_t glyphBufferSize = font1->charsize * 256;
            if (font1->mode == 1) // 512 glyph mode
                glyphBufferSize = font1->charsize * 512;
            void *glyphBuffer = (Start + sizeof(PSF1_HEADER));
            PSF1Font->Header = font1;
            PSF1Font->GlyphBuffer = glyphBuffer;
            UNUSED(glyphBufferSize); // TODO: Use this in the future?

            // guessing
            width = 16;
            height = 8;
        }
    }

    Font::~Font()
    {
    }

    FontSize Font::GetFontSize()
    {
        FontSize font = {.Width = width, .Height = height};
        return font;
    }

    FontType Font::GetFontType()
    {
        return type;
    }

    Display::Display(bool LoadDefaultFont)
    {
        trace("Initializing Display object...");
        framebuffer = new Framebuffer;
        framebuffer->Address = bootparams->Framebuffer.BaseAddress;
        framebuffer->PixelsPerScanLine = bootparams->Framebuffer.PixelsPerScanLine;
        framebuffer->Width = bootparams->Framebuffer.Width;
        framebuffer->Height = bootparams->Framebuffer.Height;
        framebuffer->Size = bootparams->Framebuffer.BufferSize;

        if (LoadDefaultFont)
        {
            trace("Loading default font...");
            // TODO: Fix PSF1 fonts
            // TODO: Implement an easy way to change psf fonts
            // CurrentFont = new Font(&_binary_files_zap_ext_vga16_psf_start, &_binary_files_zap_ext_vga16_psf_end, FontType::PCScreenFont1);
            CurrentFont = new Font(&_binary_files_ter_powerline_v12n_psf_start, &_binary_files_ter_powerline_v12n_psf_end, FontType::PCScreenFont2);
            // CurrentFont = new Font(&_binary_files_zap_ext_light20_psf_start, &_binary_files_zap_ext_light20_psf_end, FontType::PCScreenFont2);
            // CurrentFont = new Font(&_binary_files_zap_ext_light24_psf_start, &_binary_files_zap_ext_light24_psf_end, FontType::PCScreenFont2);
        }
    }

    Display::~Display()
    {
        delete CurrentFont;
    }

    void Display::Scroll() { memmove((void *)framebuffer->Address, (void *)(framebuffer->Address + framebuffer->Width * CurrentFont->GetFontSize().Height * 4), framebuffer->Width * (framebuffer->Height - CurrentFont->GetFontSize().Height) * 4); }

    char Display::KernelPrint(char Char)
    {
        if (Char == '\n')
        {
            ploc.Y += CurrentFont->GetFontSize().Height;
            if (ploc.Y >= (framebuffer->Height - CurrentFont->GetFontSize().Height))
            {
                Scroll();
                ploc.Y = (ploc.Y - CurrentFont->GetFontSize().Height);
            }
            ploc.X = 0;
            return Char;
        }
        else if (Char == '\r')
        {
            return Char;
        }
        // TODO: add better support for unsupported characters
        if (Char < 0 || (unsigned char)Char > 127)
            Char = '?';

        if (ploc.X + 8 > framebuffer->Width)
        {
            ploc.X = 0;
            ploc.Y += CurrentFont->GetFontSize().Height;
            if (ploc.Y >= (framebuffer->Height - CurrentFont->GetFontSize().Height))
            {
                Scroll();
                ploc.Y = (ploc.Y - CurrentFont->GetFontSize().Height);
            }
        }

        if (CurrentFont->GetFontType() == FontType::PCScreenFont2)
        {
            // if (CurrentFont->PSF2Font->GlyphBuffer == (uint16_t *)0x01) // HAS UNICODE TABLE
            //     Char = CurrentFont->PSF2Font->GlyphBuffer[Char];
            int bytesperline = (CurrentFont->PSF2Font->Header->width + 7) / 8;
            uint32_t *PixelPtr = (uint32_t *)framebuffer->Address;
            char *FontPtr = (char *)CurrentFont->PSFFile.start + CurrentFont->PSF2Font->Header->headersize + (Char > 0 && (unsigned char)Char < CurrentFont->PSF2Font->Header->length ? Char : 0) * CurrentFont->PSF2Font->Header->charsize;
            for (unsigned long Y = ploc.Y; Y < ploc.Y + CurrentFont->PSF2Font->Header->height; Y++)
            {
                for (unsigned long X = ploc.X; X < ploc.X + CurrentFont->PSF2Font->Header->width; X++)
                    if ((*FontPtr & (0b10000000 >> (X - ploc.X))) > 0)
                        *(uint32_t *)(PixelPtr + X + (Y * framebuffer->PixelsPerScanLine)) = this->color;
                FontPtr += bytesperline;
            }
            ploc.X += CurrentFont->PSF2Font->Header->width;
            return Char;
        }
        else if (CurrentFont->GetFontType() == FontType::PCScreenFont1)
        {
            uint32_t *PixelPtr = (uint32_t *)framebuffer->Address;
            char *FontPtr = (char *)CurrentFont->PSF1Font->GlyphBuffer + (Char * CurrentFont->PSF1Font->Header->charsize);
            for (unsigned long Y = ploc.Y; Y < ploc.Y + 16; Y++)
            {
                for (unsigned long X = ploc.X; X < ploc.X + 8; X++)
                    if ((*FontPtr & (0b10000000 >> (X - ploc.X))) > 0)
                        *(unsigned int *)(PixelPtr + X + (Y * framebuffer->Width)) = this->color;
                FontPtr++;
            }
            ploc.X += 8;
            return Char;
        }
        return Char;
    }

    void Display::SetPixel(uint32_t X, uint32_t Y, uint32_t Color)
    {
        if (X >= framebuffer->Width || Y >= framebuffer->Height)
            return;
        ((uint32_t *)framebuffer->Address)[X + (Y * framebuffer->PixelsPerScanLine)] = Color;
    }

    uint32_t Display::GetPixel(uint32_t X, uint32_t Y)
    {
        if (X >= framebuffer->Width || Y >= framebuffer->Height)
            return 0x00000000;
        return ((uint32_t *)framebuffer->Address)[X + (Y * framebuffer->PixelsPerScanLine)];
    }

    void Display::Clear(uint32_t Color)
    {
        for (uint32_t VerticalScanline = 0; VerticalScanline < framebuffer->Height; VerticalScanline++)
        {
            uint64_t PixelPtrBase = framebuffer->Address + ((framebuffer->PixelsPerScanLine * 4) * VerticalScanline);
            for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (framebuffer->PixelsPerScanLine * 4)); PixelPtr++)
                *PixelPtr = Color;
        }
        ploc = {.X = 0, .Y = 0};
    }

    void Display::SetPrintLocation(uint32_t X, uint32_t Y)
    {
        ploc.X = X;
        ploc.Y = Y;
    }

    void Display::ResetPrintPosition()
    {
        ploc.X = 0;
        ploc.Y = 0;
    }

    void Display::SetPrintColor(uint32_t Color)
    {
        color = Color;
    }

    void Display::ResetPrintColor()
    {
        color = 0xFFFFFFFF;
    }

    Framebuffer *Display::GetFramebuffer()
    {
        return framebuffer;
    }
}

EXTERNC struct Framebuffer *GetFramebuffer()
{
    return CurrentDisplay->GetFramebuffer();
}

EXTERNC char putchar(char c)
{
    CurrentDisplay->KernelPrint(c);
    return c;
}

EXTERNC void putchar_(char c)
{
    putchar(c);
}
