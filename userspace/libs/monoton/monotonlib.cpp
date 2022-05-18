#include "monotonlib.h"
#include <syscalls.h>
#include <stdlib.h>

namespace MonotonLib
{
    mtl::mtl(char *FontPath)
    {
        syscall_dbg(0x3F8, (char *)"[MonotonLib] Started.\n");
        void *FontBinary = (void *)syscall_FileOpen(FontPath);
        if (FontBinary == nullptr)
        {
            syscall_dbg(0x3F8, (char *)"[LoadFont] Error! Could not open file.\n");
            syscall_FileClose(FontBinary);
            return;
        }
        syscall_dbg(0x3F8, (char *)"[LoadFont] File opened.\n");
        uint64_t FontFileSize = syscall_FileSize(FontBinary);
        void *FontAllocatedData = malloc(FontFileSize);
        syscall_FileRead(FontBinary, 0, FontAllocatedData, FontFileSize);

        PSF2Font = new PSF2_FONT;
        uint16_t glyph2 = 0;
        PSF2_HEADER *font2 = (PSF2_HEADER *)FontAllocatedData;
        if (font2->magic[0] != PSF2_MAGIC0 || font2->magic[1] != PSF2_MAGIC1 || font2->magic[2] != PSF2_MAGIC2 || font2->magic[3] != PSF2_MAGIC3)
        {
            syscall_dbg(0x3F8, (char *)"[LoadFont] Invalid PSF2 file.\n");
            syscall_FileClose(FontBinary);
            free(FontAllocatedData);
            delete PSF2Font;
            return;
        }

        PSF2Font->Header = font2;
        PSF2Font->GlyphBuffer = (void *)((uint64_t)FontAllocatedData + sizeof(PSF2_HEADER));
        syscall_dbg(0x3F8, (char *)"[LoadFont] Font loaded.\n");

        FB.Address = syscall_displayAddress();
        FB.Width = syscall_displayWidth();
        FB.Height = syscall_displayHeight();
        FB.PixelsPerScanLine = syscall_displayPixelsPerScanLine();

        syscall_FileClose(FontBinary);
        // free(FontAllocatedData);
        // free(PSF2Font);
    }

    mtl::~mtl()
    {
        free(PSF2Font->Header);
        delete PSF2Font;
    }

    void mtl::SetPrintPosition(PrintPos Position)
    {
        Xpos = Position.x;
        Ypos = Position.y;
    }

    PrintPos mtl::GetPrintPosition()
    {
        return {Xpos, Ypos};
    }

    long unsigned monotonlib_strlen(char s[])
    {
        long unsigned i = 0;
        while (s[i] != '\0')
            ++i;
        return i;
    }

    // void Scroll()
    // {
    //     memmove((void *)framebuffer->Address, (void *)(framebuffer->Address + framebuffer->Width * CurrentFont->GetFontSize().Height * 4), framebuffer->Width * (framebuffer->Height - CurrentFont->GetFontSize().Height) * 4);
    // }

    void mtl::print(const char *Text, PrintPos Position)
    {
        uint64_t Xi = Position.x, Yi = Position.y;

        for (int i = 0; i < monotonlib_strlen((char *)Text); i++)
        {
            char Char = Text[i];
            if (Char == '\n')
            {
                Xi = 0;
                Yi += PSF2Font->Header->height;
                continue;
            }
            else if (Char == '\r')
            {
                Xi = 0;
                continue;
            }
            if (Char < 0 || (unsigned char)Char > 127)
                Char = '?';
            int bytesperline = (PSF2Font->Header->width + 7) / 8;
            uint32_t *PixelPtr = (uint32_t *)FB.Address;
            char *FontPtr = (char *)PSF2Font->Header + PSF2Font->Header->headersize + (Char > 0 && (unsigned char)Char < PSF2Font->Header->length ? Char : 0) * PSF2Font->Header->charsize;
            for (unsigned long Y = Yi; Y < Yi + PSF2Font->Header->height; Y++)
            {
                for (unsigned long X = Xi; X < Xi + PSF2Font->Header->width; X++)
                    if ((*FontPtr & (0b10000000 >> (X - Xi))) > 0)
                        *(uint32_t *)(PixelPtr + X + (Y * FB.PixelsPerScanLine)) = 0xFFFFFF;
                FontPtr += bytesperline;
            }
            Xi += PSF2Font->Header->width;
        }

        Xpos = Xi;
        Ypos = Yi;
    }

    void mtl::print(const char *Text)
    {
        this->print(Text, {Xpos, Ypos});
    }
}
