#include <driver.h>
#include <interrupts.h>

DriverKernelMainData *Calls = nullptr;

InterruptHandler(KeyboardInterrupt)
{
    Calls->KFctCall(KCALL_END_OF_INTERRUPT, INT_NUM);
}

DRIVER_ENTRY
{
    Calls = Data;
    Data->KFctCall(KCALL_HOOK_INTERRUPT, IRQ1, KeyboardInterrupt);
    return DRIVER_SUCCESS;
}

DRIVER = {
    .Name = "PS/2 Keyboard Driver",
    .Type = TYPE_KEYBOARD,
    .Bind =
        {
            .Bind = BIND_INTERRUPT,
            .Interrupt =
                {
                    .Vector = IRQ1}}};
