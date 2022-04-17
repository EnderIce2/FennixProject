#include <bootscreen.h>
#include <debug.h>

BootScreen::Screen *BS = nullptr;

namespace BootScreen
{
    Screen::Screen()
    {

    }

    Screen::~Screen()
    {
        warn("Tried to uninitialize the Boot Screen!");
    }
}