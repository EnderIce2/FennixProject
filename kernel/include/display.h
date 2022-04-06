#pragma once
#include <types.h>

struct Framebuffer
{
    uint64_t Address;
    uint64_t Size;
    uint32_t Width, Height;
    uint32_t PixelsPerScanLine;
};

#ifdef __cplusplus

namespace DisplayDriver
{
    enum FontType
    {
        None,
        PCScreenFont1,
        PCScreenFont2
    };

    struct FontSize
    {
        // Width of the font in pixels
        uint8_t Width;
        // Height of the font in pixels
        uint8_t Height;
    };

    class Font
    {
    public:
        /**
         * @brief Construct a new PC Screen Font object
         * @param address The address of the font
         * @param type The font type
         */
        Font(uint64_t address, FontType type);
        /**
         * @brief Destroy the Font object
         */
        ~Font();
        /**
         * @brief Get the font size
         * @return FontSize (Width, Height)
         */
        FontSize GetFontSize();

    private:
        uint8_t width;
        uint8_t height;
    };

    struct DisplayProperties
    {
        // TODO: add more properties
        int Width;
        int Height;
    };

    class Display
    {
    public:
        /**
         * @brief Initialized by DisplayDriver::Display()
         */
        DisplayDriver::Font *CurrentFont = nullptr;
        /**
         * @brief Current display properties
         */
        DisplayProperties Properties;
        /**
         * @brief Construct a new Display object
         */
        Display();
        /**
         * @brief Destroy the Display object
         */
        ~Display();
        /**
         * @brief Print on screen text
         * @brief DEPRECATED: Use printf from printf.h instead
         * @param format Format string
         * @param ... Arguments
         */
        void KernelPrint(string format, ...);
        /**
         * @brief Set print location
         * @param x
         * @param y
         */
        void SetPrintLocation(uint32_t X, uint32_t Y);
        /**
         * @brief Reset text position
         */
        void ResetPrintPosition();
        /**
         * @brief Set print color
         * @param color 0xAARRGGBB
         */
        void SetPrintColor(uint32_t Color);
        /**
         * @brief Reset print color to the default value. (0xFFFFFFFF)
         */
        void ResetPrintColor();
        /**
         * @brief Get the Framebuffer object
         * @return Framebuffer
         */
        Framebuffer GetFramebuffer();

    private:
        uint32_t x;
        uint32_t y;
        uint32_t color;
        Framebuffer framebuffer;
    };
}

/**
 * @brief Current display information
 */
extern DisplayDriver::Display CurrentDisplay;

#endif

START_EXTERNC

struct Framebuffer *GetFramebuffer();
char putchar(char c);
// compatibility the printf implementation
void putchar_(char c);

END_EXTERNC
