#pragma once
#include <types.h>

// TODO: Add a bootscreen and also take advantage of the fact that we have BGRT implemented.
// ! Do not use anything that has to do with memory allocation.

namespace BootScreen
{
    class Screen
    {
        Screen();
        ~Screen();
    };
}

extern BootScreen::Screen *BS;