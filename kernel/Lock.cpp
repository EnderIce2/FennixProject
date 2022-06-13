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
    printf("Potential deadlock in lock '%s' held by '%s'", lock->lock_name, lock->current_holder);
    CurrentDisplay->ResetPrintColor();
    CurrentDisplay->SetPrintLocation(x, y);
    serial_write_text(COM1, (char *)"Potential deadlock in lock '");
    serial_write_text(COM1, (char *)lock->lock_name);
    serial_write_text(COM1, (char *)"' held by '");
    serial_write_text(COM1, (char *)lock->current_holder);
    serial_write_text(COM1, (char *)"'\nAttempting to get lock from ");
    serial_write_text(COM1, (char *)lock->attempting_to_get);
    write_serial(COM1, '\n');
    timeout++;
    if (timeout == 20)
    {
        timeout = 0;
        // Unlock it. Maybe this is a bad idea so that's why
        // TODO: Unlock only if it's from the kernel. If a driver or something else is holding it, we should not unlock after timeout. Also this never should be enabled when the smp is enabled. Or at least a timeout should be per core.
        spinlock_unlock(&lock->lock_dat);
        lock->count--;
    }
}

extern "C" unsigned long api_redirect_debug_write(int type, char *message, const char *file, int line, const char *function, ...)
{
    return debug(message);
}
