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
#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

    struct PSF1_HEADER
    {
        uint8_t magic[2];
        uint8_t mode;
        uint8_t charsize;
    };

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

    typedef struct _PSF1_FONT
    {
        PSF1_HEADER *Header;
        void *GlyphBuffer;
    } PSF1_FONT;

    typedef struct _PSF2_FONT
    {
        PSF2_HEADER *Header;
        void *GlyphBuffer;
    } PSF2_FONT;

    struct PSFFileInfo
    {
        uint64_t *start;
        uint64_t *end;
    };

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
        PSFFileInfo PSFFile;
        PSF1_FONT *PSF1Font;
        PSF2_FONT *PSF2Font;

        /**
         * @brief Construct a new PC Screen Font object
         * @param address The address of the font
         * @param type The font type
         */
        Font(uint64_t *Start, uint64_t *End, FontType Type);
        /**
         * @brief Destroy the Font object
         */
        ~Font();
        /**
         * @brief Get font size
         * @return FontSize (Width, Height)
         */
        FontSize GetFontSize();
        /**
         * @brief Get font type
         * @return FontType
         */
        FontType GetFontType();

    private:
        uint8_t width;
        uint8_t height;
        FontType type;
    };

    struct DisplayProperties
    {
        // TODO: add more properties
        int Width;
        int Height;
    };

    struct PrintLocation
    {
        unsigned int X;
        unsigned int Y;
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
        Display(bool LoadDefaultFont = true);
        /**
         * @brief Destroy the Display object
         */
        ~Display();
        /**
         * @brief Print a char on screen
         * @param format Format string
         * @param ... Arguments
         */
        char KernelPrint(char Char);
        /**
         * @brief Clear the screen
         */
        void Clear(uint32_t Color = 0x00000000);
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
         * @brief Get the Print Location object
         * @return PrintLocation 
         */
        PrintLocation GetPrintLocation() { return this->ploc; }
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
        Framebuffer *GetFramebuffer();

        void SetPixel(uint32_t X, uint32_t Y, uint32_t Color);
        uint32_t GetPixel(uint32_t X, uint32_t Y);

    private:
        PrintLocation ploc = {.X = 0, .Y = 0};
        uint32_t color = 0xFFFFFFFF;
        Framebuffer *framebuffer;
        void Scroll();
    };
}

/**
 * @brief Current display information
 */
extern DisplayDriver::Display *CurrentDisplay;

#endif

START_EXTERNC

struct Framebuffer *GetFramebuffer();
char putchar(char c);
// compatibility the printf implementation
void putchar_(char c);

#include <printf.h> // TODO: PRINTF_VISIBILITY??????
#define printf(format, ...) printf_(format, ##__VA_ARGS__)

END_EXTERNC
