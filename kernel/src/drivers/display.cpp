#include <display.h>
#include <stdarg.h>
#include <printf.h>
#include <binaries.h>

DisplayDriver::Display CurrentDisplay;

namespace DisplayDriver
{
    Font::Font(uint64_t address, FontType type)
    {
    }

    Font::~Font()
    {
    }

    FontSize Font::GetFontSize()
    {
        FontSize font = {width, height};
        return font;
    }

    Display::Display()
    {
        CurrentFont = new Font(zap_ext_vga16, FontType::PCScreenFont1);
    }

    Display::~Display()
    {
        delete CurrentFont;
    }

    void Display::KernelPrint(string format, ...)
    {
        va_list args;
        va_start(args, format);
        // vprintf_(format, args);
        va_end(args);
    }

    void Display::SetPrintLocation(uint32_t X, uint32_t Y)
    {
        x = X;
        y = Y;
    }

    void Display::ResetPrintPosition()
    {
        x = 0;
        y = 0;
    }

    void Display::SetPrintColor(uint32_t Color)
    {
        color = Color;
    }

    void Display::ResetPrintColor()
    {
        color = 0xFFFFFFFF;
    }

    Framebuffer Display::GetFramebuffer()
    {
        return framebuffer;
    }
}

EXTERNC struct Framebuffer *GetFramebuffer()
{
    // Not returning the display structure because i'm not sure if it's the same.
    struct Framebuffer fb =
        {
            .Address = CurrentDisplay.GetFramebuffer().Address,
            .Size = CurrentDisplay.GetFramebuffer().Size,
            .Width = CurrentDisplay.GetFramebuffer().Width,
            .Height = CurrentDisplay.GetFramebuffer().Height,
            .PixelsPerScanLine = CurrentDisplay.GetFramebuffer().PixelsPerScanLine,
        };
    return &fb;
}

EXTERNC char putchar(char c)
{
    CurrentDisplay.KernelPrint("%c", c);
    return c;
}

EXTERNC void putchar_(char c)
{
    CurrentDisplay.KernelPrint("%c", c);
}