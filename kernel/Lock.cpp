#include <lock.h>
#include <display.h>
#include <debug.h>
#include "drivers/serial.h"

static int timeout = 0;

// TODO: complete implementation
extern "C" void deadlock_handler(LOCK *lock)
{
    uint32_t x, y;
    x = CurrentDisplay->CurrentFont->GetFontSize().Width;
    y = CurrentDisplay->CurrentFont->GetFontSize().Height;
    CurrentDisplay->ResetPrintPosition();
    CurrentDisplay->SetPrintColor(0x450402);
    printf("Potential deadlock in lock '%s' held by '%s'", lock->LockName, lock->CurrentHolder);
    CurrentDisplay->ResetPrintColor();
    CurrentDisplay->SetPrintLocation(x, y);
    serial_write_text(COM1, (char *)"Potential deadlock in lock '");
    serial_write_text(COM1, (char *)lock->LockName);
    serial_write_text(COM1, (char *)"' held by '");
    serial_write_text(COM1, (char *)lock->CurrentHolder);
    serial_write_text(COM1, (char *)"'\nAttempting to get lock from ");
    serial_write_text(COM1, (char *)lock->AttemptingToGet);
    write_serial(COM1, '\n');
    timeout++;
    if (timeout == 20)
    {
        timeout = 0;
        spinlock_unlock(&lock->LockData);
        lock->Count--;
    }
}

extern "C" unsigned long api_redirect_debug_write(int type, char *message, const char *file, int line, const char *function, ...)
{
    return debug(message);
}
