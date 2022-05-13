#include <bootscreen.h>
#include <display.h>
#include <printf.h>
#include <heap.h>
#include <debug.h>
#include "../cpu/acpi.hpp"

extern uint64_t _binary_files_zap_ext_light20_psf_start;
extern uint64_t _binary_files_zap_ext_light20_psf_end;

using namespace DisplayDriver;

BootScreen::Screen *BS = nullptr;

namespace BootScreen
{
    Display *BootDisplay = nullptr;

    static inline void bootscreenprint_wrapper(char c, void *unused)
    {
        (void)unused;
        BootDisplay->KernelPrint(c);
    }

    int bootscreenhelper_vprintf(const char *format, va_list list) { return vfctprintf(bootscreenprint_wrapper, NULL, format, list); }

    void bootscreenprint(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        bootscreenhelper_vprintf(format, args);
        va_end(args);
    }

#define SET_PRINT_MID(text, y)                                                                                                                     \
    BootDisplay->SetPrintLocation((BootDisplay->GetFramebuffer()->Width - (strlen(text) * BootDisplay->CurrentFont->GetFontSize().Width)) / 2, y); \
    bootscreenprint(text)
#define SET_PRINT_RIGHT(text, y)                                                                                                             \
    BootDisplay->SetPrintLocation(BootDisplay->GetFramebuffer()->Width - (strlen(text) * BootDisplay->CurrentFont->GetFontSize().Width), y); \
    bootscreenprint(text)

#define SET_MID(text, y) BootDisplay->SetPrintLocation((BootDisplay->GetFramebuffer()->Width - (strlen(text) * BootDisplay->CurrentFont->GetFontSize().Width)) / 2, y)
#define SET_RIGHT(text, y) BootDisplay->SetPrintLocation(BootDisplay->GetFramebuffer()->Width - (strlen(text) * BootDisplay->CurrentFont->GetFontSize().Width), y)

    void Screen::DrawVendorLogo(void *BGRT)
    {
        ACPI::ACPI::BGRTHeader *bgrt = (ACPI::ACPI::BGRTHeader *)BGRT;
        if (bgrt->Version >= 1)
        {
            if (bgrt->ImageType == 0)
            {
                trace("Boot Graphics Record Table info:\tVersion:%d\tStatus:%d\tImage Type:%d\tImage Address:%#llx\tOffsetX/Y: %d/%d",
                      bgrt->Version, bgrt->Status, bgrt->ImageType, bgrt->ImageAddress, bgrt->ImageOffsetX, bgrt->ImageOffsetY);

                if (bgrt->Status & 0b00)
                {
                    trace("Image no offset");
                }
                else if (bgrt->Status & 0b01)
                {
                    trace("Image 90 degrees offset");
                }
                else if (bgrt->Status & 0b10)
                {
                    trace("Image 180 degrees offset");
                }
                else if (bgrt->Status & 0b11)
                {
                    trace("Image 270 degrees offset");
                }

                // http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html
                uint8_t ImageHeader[54];
                int BufferRead = 54;
                int BufferIndex = 0;
                uint8_t *ImageBuffer = (uint8_t *)bgrt->ImageAddress;

                while (BufferRead > 0)
                {
                    ImageHeader[BufferIndex] = *ImageBuffer;
                    ImageBuffer++;
                    BufferIndex++;
                    BufferRead--;
                }

                int ImageWidth = *(int *)&ImageHeader[18];
                int ImageHeight = *(int *)&ImageHeader[22];

                uint8_t *ImageData = (uint8_t *)bgrt->ImageAddress;
                ImageData += *(int *)&ImageHeader[10];

                for (int i = ImageHeight; 0 < i; i--)
                    for (int j = 0; j < (ImageWidth * ImageHeight / ImageHeight); j++)
                        for (int g = 2; 0 <= g; g--)
                            ((uint8_t *)BootDisplay->GetFramebuffer()->Address)[((j + (i * BootDisplay->GetFramebuffer()->Width)) * 4 + ((((BootDisplay->GetFramebuffer()->Width / 2) - ImageWidth / 2) + ((BootDisplay->GetFramebuffer()->Height / 2) - ImageHeight) * BootDisplay->GetFramebuffer()->Width) * 4)) + g] = ImageData[((j * ImageWidth) / (ImageWidth * ImageHeight / ImageHeight) + (((ImageHeight - i) * ImageHeight) / ImageHeight) * ImageWidth) * ((*(short *)&ImageHeader[28]) / 8) + g];
            }
            else
            {
                err("Invalid image type");
            }
        }
        else
        {
            err("Invalid BGRT version");
        }
    }

    void Screen::Progress(int Value)
    {
        if (Value > 100)
            Value = 100;

        int BootBarStart = BootDisplay->GetFramebuffer()->Width / 2 - 100;
        int BootBarEnd = BootDisplay->GetFramebuffer()->Width / 2 + 100;
        int BootBarYSize = BootDisplay->GetFramebuffer()->Height / 2 + 60;

        int BarPosition = BootBarStart + Value * (BootBarEnd - BootBarStart) / 100;

        for (int y = BootBarYSize - 10; y < BootBarYSize; y++)
            for (int x = BootBarStart; x < BarPosition; x++)
                BootDisplay->SetPixel(x, y, 0xFFFFFFFF);

        for (int i = BootBarStart; i <= BootBarEnd; i++)
            for (int j = BootBarYSize - 10; j <= BootBarYSize; j++)
                if (i == BootBarStart || i == BootBarEnd || j == (BootBarYSize - 10) || j == BootBarYSize)
                    BootDisplay->SetPixel(i, j, 0xFFFFFFFF);
    }

    int curProg = 0;
    void Screen::IncreaseProgres()
    {
        {
            this->Progress(curProg);
            curProg++;
        }
    }

    Screen::Screen()
    {
        BootDisplay = new Display();
        delete BootDisplay->CurrentFont;
        BootDisplay->CurrentFont = new Font(&_binary_files_zap_ext_light20_psf_start,
                                            &_binary_files_zap_ext_light20_psf_end,
                                            FontType::PCScreenFont2);
        BootDisplay->Clear();
        char *text;
        sprintf_(text, "%s %s", KERNEL_NAME, KERNEL_VERSION);
        SET_PRINT_RIGHT((char *)text, BootDisplay->GetFramebuffer()->Height - BootDisplay->CurrentFont->GetFontSize().Height);
    }

    Screen::~Screen()
    {
        warn("Tried to uninitialize the Boot Screen!");
    }
}
