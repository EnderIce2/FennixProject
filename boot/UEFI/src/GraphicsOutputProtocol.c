#include "GraphicsOutputProtocol.h"

void InitGOP(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes = 0, nativeMode = 0;
    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void **)&gop);
    if (EFI_ERROR(status))
    {
        Print(L"Unable to locate the Graphics Output Protocol.\n");
        return;
    }
    status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED)
    {
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    }
    if (EFI_ERROR(status))
    {
        Print(L"Unable to get native mode.\n");
    }
    else
    {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
    }
    for (UINTN i = 0; i < numModes; i++)
    {
        status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
        Print(L"Mode %03d: %dx%d\t Format:%x Current:%d\n",
              i,
              info->HorizontalResolution,
              info->VerticalResolution,
              info->PixelFormat,
              i == nativeMode ? 1 : 0);

        // set 1280x720 mode if available
        if (info->HorizontalResolution == 1280 && info->VerticalResolution == 720)
            if (EFI_ERROR(uefi_call_wrapper(gop->SetMode, 2, gop, i)))
                Print(L"Unable to set mode %03d\n", i);
    }

    if (EFI_ERROR(status))
    {
        Print(L"An error occurred while initalizing Graphics Output Protocol.\n");
    }
    else
    {
        Print(L"Graphics Output Protocol Initialized.\n");
    }
    // return &framebuffer;
}
