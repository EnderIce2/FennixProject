#include "monotonlib.h"
#include <syscalls.h>
#include <alloc.h>
#include <string.h>

namespace MonotonLib
{
    mtl::mtl(char *FontPath)
    {
        syscall_dbg(0x3F8, (char *)"[MonotonLib] Started.\n");
        File *FontBinary = (File *)syscall_FileOpen(FontPath);
        if (FontBinary == nullptr)
        {
            syscall_dbg(0x3F8, (char *)"[LoadFont] Error! Could not open file.\n");
            syscall_FileClose(FontBinary);
            return;
        }
        syscall_dbg(0x3F8, (char *)"[LoadFont] File opened.\n");
        uint64_t FontFileSize = FontBinary->Length;
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
    }

    mtl::~mtl()
    {
        free(PSF2Font->Header);
        delete PSF2Font;
    }

    void mtl::SetBackgroundColor(uint32_t Color)
    {
        this->Background = Color;
    }

    void mtl::SetForegroundColor(uint32_t Color)
    {
        this->Foreground = Color;
    }

    void mtl::SetPrintPosition(PrintPos Position)
    {
        if (Position.x > FB.Width)
        {
            Xpos = FB.Width;
            return;
        }
        else if (Position.y > FB.Height)
        {
            Ypos = FB.Height;
            return;
        }
        else if (Position.x < 0)
        {
            Xpos = 0;
            return;
        }
        else if (Position.y < 0)
        {
            Ypos = 0;
            return;
        }
        Xpos = Position.x;
        Ypos = Position.y;
    }

    PrintPos mtl::GetPrintPosition()
    {
        return {Xpos, Ypos};
    }

    void mtl::Clear()
    {
        for (int VerticalScanline = 0; VerticalScanline < FB.Height; VerticalScanline++)
        {
            uint64_t PixelPtrBase = FB.Address + ((FB.PixelsPerScanLine * 4) * VerticalScanline);
            for (uint32_t *PixelPtr = (uint32_t *)PixelPtrBase; PixelPtr < (uint32_t *)(PixelPtrBase + (FB.PixelsPerScanLine * 4)); PixelPtr++)
                *PixelPtr = Background;
        }
        this->SetPrintPosition({0, 0});
    }

    void mtl::Scroll()
    {
        memmove((void *)FB.Address,
                (void *)(FB.Address + static_cast<uint64_t>(FB.Width) * PSF2Font->Header->height * 4),
                static_cast<uint64_t>(FB.Width) * (FB.Height - PSF2Font->Header->height) * 4);
    }

    void mtl::RemoveChar()
    {
        for (unsigned long Y = Ypos; Y < Ypos + PSF2Font->Header->height; Y++)
            for (unsigned long X = Xpos - PSF2Font->Header->width; X < Xpos; X++)
                *(unsigned int *)((unsigned int *)FB.Address + X + (Y * FB.PixelsPerScanLine)) = Background;
        this->SetPrintPosition({Xpos - PSF2Font->Header->width, Ypos});
    }

    void mtl::RemoveChar(uint32_t x, uint32_t y)
    {
        for (unsigned long Y = y; Y < y + PSF2Font->Header->height; Y++)
            for (unsigned long X = x - PSF2Font->Header->width; X < x; X++)
                *(unsigned int *)((unsigned int *)FB.Address + X + (Y * FB.PixelsPerScanLine)) = Background;
    }

    void mtl::printchar(char Char, PrintPos Position)
    {
        uint32_t Xi = Position.x, Yi = Position.y;

        if (Char == '\n')
        {
            Xi = 0;
            Yi += PSF2Font->Header->height;
            this->SetPrintPosition({Xi, Yi});
            if (Ypos >= (FB.Height - PSF2Font->Header->height))
            {
                this->Scroll();
                this->SetPrintPosition({Xpos, Ypos - PSF2Font->Header->height});
            }
            return;
        }
        else if (Char == '\r')
        {
            Xi = 0;
            this->SetPrintPosition({Xi, Yi});
            return;
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
                    *(uint32_t *)(PixelPtr + X + (Y * FB.PixelsPerScanLine)) = Foreground;
            FontPtr += bytesperline;
        }
        Xi += PSF2Font->Header->width;
        this->SetPrintPosition({Xi, Yi});
    }

    void mtl::printchar(char Char)
    {
        if (Char == '\n')
        {
            this->SetPrintPosition({0, Ypos + PSF2Font->Header->height});
            if (Ypos >= (FB.Height - PSF2Font->Header->height))
            {
                this->Scroll();
                this->SetPrintPosition({Xpos, Ypos - PSF2Font->Header->height});
            }
            return;
        }
        else if (Char == '\r')
        {
            this->SetPrintPosition({0, Ypos});
            return;
        }
        if (Char < 0 || (unsigned char)Char > 127)
            Char = '?';
        int bytesperline = (PSF2Font->Header->width + 7) / 8;
        uint32_t *PixelPtr = (uint32_t *)FB.Address;
        char *FontPtr = (char *)PSF2Font->Header + PSF2Font->Header->headersize + (Char > 0 && (unsigned char)Char < PSF2Font->Header->length ? Char : 0) * PSF2Font->Header->charsize;
        for (unsigned long Y = Ypos; Y < Ypos + PSF2Font->Header->height; Y++)
        {
            for (unsigned long X = Xpos; X < Xpos + PSF2Font->Header->width; X++)
                if ((*FontPtr & (0b10000000 >> (X - Xpos))) > 0)
                    *(uint32_t *)(PixelPtr + X + (Y * FB.PixelsPerScanLine)) = Foreground;
            FontPtr += bytesperline;
        }
        Xpos += PSF2Font->Header->width;
    }

    void mtl::print(const char *Text, PrintPos Position)
    {
        uint32_t Xi = Position.x, Yi = Position.y;
        long unsigned textlength = 0;

        while (Text[textlength] != '\0')
            ++textlength;

        for (int i = 0; i < textlength; i++)
        {
            char Char = Text[i];
            if (Char == '\n')
            {
                Xi = 0;
                Yi += PSF2Font->Header->height;
                if (Ypos >= (FB.Height - PSF2Font->Header->height))
                {
                    this->Scroll();
                    this->SetPrintPosition({Xpos, Ypos - PSF2Font->Header->height});
                }
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
                        *(uint32_t *)(PixelPtr + X + (Y * FB.PixelsPerScanLine)) = Foreground;
                FontPtr += bytesperline;
            }
            Xi += PSF2Font->Header->width;
        }
        this->SetPrintPosition({Xi, Yi});
    }

    void mtl::print(const char *Text)
    {
        this->print(Text, {Xpos, Ypos});
    }
}
