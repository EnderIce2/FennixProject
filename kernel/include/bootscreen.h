#pragma once
#include <types.h>

namespace BootScreen
{
    class Screen
    {
    private:
        bool KernelLogo;
        int curProg = 0;

    public:
        void DrawVendorLogo(void *BGRT, bool DrawKernelLogo = true);
        void Progress(int Value);
        void IncreaseProgres();
        void FadeLogo();
        Screen();
        ~Screen();
    };
}

extern BootScreen::Screen *BS;