#pragma once
#include <types.h>

namespace BootScreen
{
    class Screen
    {
    public:
        void DrawVendorLogo(void *BGRT, bool DrawKernelLogo = true);
        void Progress(int Value);
        void IncreaseProgres();
        Screen();
        ~Screen();
    };
}

extern BootScreen::Screen *BS;