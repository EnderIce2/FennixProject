#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    Print(L"Lynx Bootloader © EnderIce2 2022\n");
    while (1)
        asm("hlt");
    return EFI_SUCCESS;
}
