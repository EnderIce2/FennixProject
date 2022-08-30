#include <bootscreen.h>

#include <critical.hpp>
#include <display.h>
#include <printf.h>
#include <debug.h>
#include <heap.h>

#include "../cpu/acpi.hpp"
#include "../timer.h"

extern uint64_t _binary_files_zap_ext_light20_psf_start;
extern uint64_t _binary_files_zap_ext_light20_psf_end;

using namespace DisplayDriver;

BootScreen::Screen *BS = nullptr;

// black
extern uint64_t _binary_files_fennix001_bmp_end;
extern uint64_t _binary_files_fennix001_bmp_size;
extern uint64_t _binary_files_fennix001_bmp_start;
extern uint64_t _binary_files_fennix002_bmp_end;
extern uint64_t _binary_files_fennix002_bmp_size;
extern uint64_t _binary_files_fennix002_bmp_start;

// slide animation
extern uint64_t _binary_files_fennix003_bmp_end;
extern uint64_t _binary_files_fennix003_bmp_size;
extern uint64_t _binary_files_fennix003_bmp_start;
extern uint64_t _binary_files_fennix004_bmp_end;
extern uint64_t _binary_files_fennix004_bmp_size;
extern uint64_t _binary_files_fennix004_bmp_start;
extern uint64_t _binary_files_fennix005_bmp_end;
extern uint64_t _binary_files_fennix005_bmp_size;
extern uint64_t _binary_files_fennix005_bmp_start;
extern uint64_t _binary_files_fennix006_bmp_end;
extern uint64_t _binary_files_fennix006_bmp_size;
extern uint64_t _binary_files_fennix006_bmp_start;
extern uint64_t _binary_files_fennix007_bmp_end;
extern uint64_t _binary_files_fennix007_bmp_size;
extern uint64_t _binary_files_fennix007_bmp_start;
extern uint64_t _binary_files_fennix008_bmp_end;
extern uint64_t _binary_files_fennix008_bmp_size;
extern uint64_t _binary_files_fennix008_bmp_start;
extern uint64_t _binary_files_fennix009_bmp_end;
extern uint64_t _binary_files_fennix009_bmp_size;
extern uint64_t _binary_files_fennix009_bmp_start;
extern uint64_t _binary_files_fennix010_bmp_end;
extern uint64_t _binary_files_fennix010_bmp_size;
extern uint64_t _binary_files_fennix010_bmp_start;
extern uint64_t _binary_files_fennix011_bmp_end;
extern uint64_t _binary_files_fennix011_bmp_size;
extern uint64_t _binary_files_fennix011_bmp_start;
extern uint64_t _binary_files_fennix012_bmp_end;
extern uint64_t _binary_files_fennix012_bmp_size;
extern uint64_t _binary_files_fennix012_bmp_start;
extern uint64_t _binary_files_fennix013_bmp_end;
extern uint64_t _binary_files_fennix013_bmp_size;
extern uint64_t _binary_files_fennix013_bmp_start;
extern uint64_t _binary_files_fennix014_bmp_end;
extern uint64_t _binary_files_fennix014_bmp_size;
extern uint64_t _binary_files_fennix014_bmp_start;

// logo
extern uint64_t _binary_files_fennix015_bmp_end;
extern uint64_t _binary_files_fennix015_bmp_size;
extern uint64_t _binary_files_fennix015_bmp_start;

// fade
extern uint64_t _binary_files_fennix016_bmp_end;
extern uint64_t _binary_files_fennix016_bmp_size;
extern uint64_t _binary_files_fennix016_bmp_start;
extern uint64_t _binary_files_fennix017_bmp_end;
extern uint64_t _binary_files_fennix017_bmp_size;
extern uint64_t _binary_files_fennix017_bmp_start;
extern uint64_t _binary_files_fennix018_bmp_end;
extern uint64_t _binary_files_fennix018_bmp_size;
extern uint64_t _binary_files_fennix018_bmp_start;
extern uint64_t _binary_files_fennix019_bmp_end;
extern uint64_t _binary_files_fennix019_bmp_size;
extern uint64_t _binary_files_fennix019_bmp_start;
extern uint64_t _binary_files_fennix020_bmp_end;
extern uint64_t _binary_files_fennix020_bmp_size;
extern uint64_t _binary_files_fennix020_bmp_start;
extern uint64_t _binary_files_fennix021_bmp_end;
extern uint64_t _binary_files_fennix021_bmp_size;
extern uint64_t _binary_files_fennix021_bmp_start;
extern uint64_t _binary_files_fennix022_bmp_end;
extern uint64_t _binary_files_fennix022_bmp_size;
extern uint64_t _binary_files_fennix022_bmp_start;
extern uint64_t _binary_files_fennix023_bmp_end;
extern uint64_t _binary_files_fennix023_bmp_size;
extern uint64_t _binary_files_fennix023_bmp_start;
extern uint64_t _binary_files_fennix024_bmp_end;
extern uint64_t _binary_files_fennix024_bmp_size;
extern uint64_t _binary_files_fennix024_bmp_start;
extern uint64_t _binary_files_fennix025_bmp_end;
extern uint64_t _binary_files_fennix025_bmp_size;
extern uint64_t _binary_files_fennix025_bmp_start;

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

    void Screen::DrawVendorLogo(void *BGRT, bool DrawKernelLogo)
    {
        KernelLogo = DrawKernelLogo;
        ACPI::ACPI::BGRTHeader *bgrt = (ACPI::ACPI::BGRTHeader *)BGRT;
        uint64_t FBAddress = BootDisplay->GetFramebuffer()->Address;
        uint64_t FBWidth = BootDisplay->GetFramebuffer()->Width;
        uint64_t FBHeight = BootDisplay->GetFramebuffer()->Height;

        if (BGRT != nullptr)
        {
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
                                ((uint8_t *)FBAddress)[((j + (i * FBWidth)) * 4 + ((((FBWidth / 2) - ImageWidth / 2) + ((FBHeight / 2) - ImageHeight) * FBWidth) * 4)) + g] = ImageData[((j * ImageWidth) / (ImageWidth * ImageHeight / ImageHeight) + (((ImageHeight - i) * ImageHeight) / ImageHeight) * ImageWidth) * ((*(short *)&ImageHeader[28]) / 8) + g];
                    return;
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

        if (DrawKernelLogo)
        {
            uint64_t LogoAddress = (uint64_t)&_binary_files_fennix001_bmp_start;
            int LogoID = 1;
            trace("Displaying kernel logo...");

            debug("bmp 001 %#lx", &_binary_files_fennix001_bmp_start);
            debug("bmp 002 %#lx", &_binary_files_fennix002_bmp_start);
            debug("bmp 003 %#lx", &_binary_files_fennix003_bmp_start);
            debug("bmp 004 %#lx", &_binary_files_fennix004_bmp_start);
            debug("bmp 005 %#lx", &_binary_files_fennix005_bmp_start);
            debug("bmp 006 %#lx", &_binary_files_fennix006_bmp_start);
            debug("bmp 007 %#lx", &_binary_files_fennix007_bmp_start);
            debug("bmp 008 %#lx", &_binary_files_fennix008_bmp_start);
            debug("bmp 009 %#lx", &_binary_files_fennix009_bmp_start);
            debug("bmp 010 %#lx", &_binary_files_fennix010_bmp_start);
            debug("bmp 011 %#lx", &_binary_files_fennix011_bmp_start);
            debug("bmp 012 %#lx", &_binary_files_fennix012_bmp_start);
            debug("bmp 013 %#lx", &_binary_files_fennix013_bmp_start);
            debug("bmp 014 %#lx", &_binary_files_fennix014_bmp_start);
            debug("bmp 015 %#lx", &_binary_files_fennix015_bmp_start);
            debug("bmp 016 %#lx", &_binary_files_fennix016_bmp_start);
            debug("bmp 017 %#lx", &_binary_files_fennix017_bmp_start);
            debug("bmp 018 %#lx", &_binary_files_fennix018_bmp_start);
            debug("bmp 019 %#lx", &_binary_files_fennix019_bmp_start);
            debug("bmp 020 %#lx", &_binary_files_fennix020_bmp_start);
            debug("bmp 021 %#lx", &_binary_files_fennix021_bmp_start);
            debug("bmp 022 %#lx", &_binary_files_fennix022_bmp_start);
            debug("bmp 023 %#lx", &_binary_files_fennix023_bmp_start);
            debug("bmp 024 %#lx", &_binary_files_fennix024_bmp_start);
            debug("bmp 025 %#lx", &_binary_files_fennix025_bmp_start);

        RedrawLogo:
            trace("Drawing logo %d", LogoID);
            msleep(50);

            uint8_t ImageHeader[54];
            int BufferRead = 54;
            int BufferIndex = 0;
            uint8_t *ImageBuffer = (uint8_t *)LogoAddress;

            while (BufferRead > 0)
            {
                ImageHeader[BufferIndex] = *ImageBuffer;
                ImageBuffer++;
                BufferIndex++;
                BufferRead--;
            }

            int ImageWidth = *(int *)&ImageHeader[18];
            int ImageHeight = *(int *)&ImageHeader[22];

            uint8_t *ImageData = (uint8_t *)LogoAddress;
            ImageData += *(int *)&ImageHeader[10];

            for (int i = ImageHeight; 0 < i; i--)
                for (int j = 0; j < (ImageWidth * ImageHeight / ImageHeight); j++)
                    for (int g = 2; 0 <= g; g--)
                        ((uint8_t *)FBAddress)[((j + (i * FBWidth)) * 4 + ((((FBWidth / 2) - ImageWidth / 2) + ((FBHeight / 2) - ImageHeight) * FBWidth) * 4)) + g] = ImageData[((j * ImageWidth) / (ImageWidth * ImageHeight / ImageHeight) + (((ImageHeight - i) * ImageHeight) / ImageHeight) * ImageWidth) * ((*(short *)&ImageHeader[28]) / 8) + g];

            switch (LogoID)
            {
            case 1:
                LogoAddress = (uint64_t)&_binary_files_fennix001_bmp_start;
                LogoID = 2;
                goto RedrawLogo;
            case 2:
                LogoAddress = (uint64_t)&_binary_files_fennix002_bmp_start;
                LogoID = 3;
                goto RedrawLogo;
            case 3:
                LogoAddress = (uint64_t)&_binary_files_fennix003_bmp_start;
                LogoID = 4;
                goto RedrawLogo;
            case 4:
                LogoAddress = (uint64_t)&_binary_files_fennix004_bmp_start;
                LogoID = 5;
                goto RedrawLogo;
            case 5:
                LogoAddress = (uint64_t)&_binary_files_fennix005_bmp_start;
                LogoID = 6;
                goto RedrawLogo;
            case 6:
                LogoAddress = (uint64_t)&_binary_files_fennix006_bmp_start;
                LogoID = 7;
                goto RedrawLogo;
            case 7:
                LogoAddress = (uint64_t)&_binary_files_fennix007_bmp_start;
                LogoID = 8;
                goto RedrawLogo;
            case 8:
                LogoAddress = (uint64_t)&_binary_files_fennix008_bmp_start;
                LogoID = 9;
                goto RedrawLogo;
            case 9:
                LogoAddress = (uint64_t)&_binary_files_fennix009_bmp_start;
                LogoID = 10;
                goto RedrawLogo;
            case 10:
                LogoAddress = (uint64_t)&_binary_files_fennix010_bmp_start;
                LogoID = 11;
                goto RedrawLogo;
            case 11:
                LogoAddress = (uint64_t)&_binary_files_fennix011_bmp_start;
                LogoID = 12;
                goto RedrawLogo;
            case 12:
                LogoAddress = (uint64_t)&_binary_files_fennix012_bmp_start;
                LogoID = 13;
                goto RedrawLogo;
            case 13:
                LogoAddress = (uint64_t)&_binary_files_fennix013_bmp_start;
                LogoID = 14;
                goto RedrawLogo;
            case 14:
                LogoAddress = (uint64_t)&_binary_files_fennix014_bmp_start;
                LogoID = 15;
                goto RedrawLogo;
            case 15:
                LogoAddress = (uint64_t)&_binary_files_fennix015_bmp_start;
                LogoID = 16;
                goto RedrawLogo;
            case 16:
                break;
            }
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
                BootDisplay->SetPixel(x, y, 0xFFFFFF);

        for (int i = BootBarStart; i <= BootBarEnd; i++)
            for (int j = BootBarYSize - 10; j <= BootBarYSize; j++)
                if (i == BootBarStart || i == BootBarEnd || j == (BootBarYSize - 10) || j == BootBarYSize)
                    BootDisplay->SetPixel(i, j, 0xFFFFFF);
    }

    void Screen::IncreaseProgres()
    {
        this->Progress(curProg);
        curProg++;
    }

    void Screen::FadeLogo()
    {
        if (!KernelLogo)
            return;
        EnterCriticalSection;
        uint64_t LogoAddress = (uint64_t)&_binary_files_fennix001_bmp_start;
        int LogoID = 1;

        uint64_t FBAddress = BootDisplay->GetFramebuffer()->Address;
        uint64_t FBWidth = BootDisplay->GetFramebuffer()->Width;
        uint64_t FBHeight = BootDisplay->GetFramebuffer()->Height;

        int BootBarStart = FBWidth / 2 - 100;
        int BootBarEnd = FBWidth / 2 + 100;
        int BootBarYSize = FBHeight / 2 + 60;
        int PBColor = 0xFFFFFF;

    RedrawLogo:
    {
        trace("Drawing logo %d", LogoID);
        msleep(50);

        uint8_t ImageHeader[54];
        int BufferRead = 54;
        int BufferIndex = 0;
        uint8_t *ImageBuffer = (uint8_t *)LogoAddress;

        while (BufferRead > 0)
        {
            ImageHeader[BufferIndex] = *ImageBuffer;
            ImageBuffer++;
            BufferIndex++;
            BufferRead--;
        }

        int ImageWidth = *(int *)&ImageHeader[18];
        int ImageHeight = *(int *)&ImageHeader[22];

        uint8_t *ImageData = (uint8_t *)LogoAddress;
        ImageData += *(int *)&ImageHeader[10];

        for (int i = ImageHeight; 0 < i; i--)
            for (int j = 0; j < (ImageWidth * ImageHeight / ImageHeight); j++)
                for (int g = 2; 0 <= g; g--)
                    ((uint8_t *)FBAddress)[((j + (i * FBWidth)) * 4 + ((((FBWidth / 2) - ImageWidth / 2) + ((FBHeight / 2) - ImageHeight) * FBWidth) * 4)) + g] = ImageData[((j * ImageWidth) / (ImageWidth * ImageHeight / ImageHeight) + (((ImageHeight - i) * ImageHeight) / ImageHeight) * ImageWidth) * ((*(short *)&ImageHeader[28]) / 8) + g];

        switch (LogoID)
        {
        case 1:
            LogoAddress = (uint64_t)&_binary_files_fennix016_bmp_start;
            LogoID = 2;
            goto RedrawLogo;
        case 2:
            LogoAddress = (uint64_t)&_binary_files_fennix017_bmp_start;
            LogoID = 3;
            goto RedrawLogo;
        case 3:
            LogoAddress = (uint64_t)&_binary_files_fennix018_bmp_start;
            LogoID = 4;
            goto RedrawLogo;
        case 4:
            LogoAddress = (uint64_t)&_binary_files_fennix019_bmp_start;
            LogoID = 5;
            goto RedrawLogo;
        case 5:
            LogoAddress = (uint64_t)&_binary_files_fennix020_bmp_start;
            LogoID = 6;
            goto RedrawLogo;
        case 6:
            LogoAddress = (uint64_t)&_binary_files_fennix021_bmp_start;
            LogoID = 7;
            goto RedrawLogo;
        case 7:
            LogoAddress = (uint64_t)&_binary_files_fennix022_bmp_start;
            LogoID = 8;
            goto RedrawLogo;
        case 8:
            LogoAddress = (uint64_t)&_binary_files_fennix023_bmp_start;
            LogoID = 9;
            goto RedrawLogo;
        case 9:
            LogoAddress = (uint64_t)&_binary_files_fennix024_bmp_start;
            LogoID = 10;
            goto RedrawLogo;
        case 10:
            LogoAddress = (uint64_t)&_binary_files_fennix025_bmp_start;
            LogoID = 11;
            goto RedrawLogo;
        case 11:
            break;
        }
    }

    RedrawProgressBar:
    {
        msleep(50);
        for (int y = BootBarYSize - 10; y < BootBarYSize + 1; y++)
            for (int x = BootBarStart; x < BootBarEnd + 1; x++)
                BootDisplay->SetPixel(x, y, PBColor);

        if (PBColor > 0x0)
        {
            PBColor -= 0x111111;
            goto RedrawProgressBar;
        }
    }

        LeaveCriticalSection;
    }

    Screen::Screen()
    {
        BootDisplay = new Display(false);
        BootDisplay->CurrentFont = new Font(&_binary_files_zap_ext_light20_psf_start,
                                            &_binary_files_zap_ext_light20_psf_end,
                                            FontType::PCScreenFont2);
        BootDisplay->Clear();
        char *text = new char[128];
#ifdef DEBUG
        sprintf_(text, "%s git-%s", KERNEL_NAME, GIT_COMMIT_SHORT);
#else
        sprintf_(text, "%s %s", KERNEL_NAME, KERNEL_VERSION);
#endif
        SET_PRINT_RIGHT((char *)text, BootDisplay->GetFramebuffer()->Height - BootDisplay->CurrentFont->GetFontSize().Height);
        delete[] text;
    }

    Screen::~Screen()
    {
        warn("Tried to uninitialize the Boot Screen!");
    }
}
