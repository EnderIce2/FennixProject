#include "FileLoader.h"

// https://wiki.osdev.org/Loading_files_under_UEFI

EFI_FILE_HANDLE GetVolume(EFI_HANDLE ImageHandle)
{
    EFI_LOADED_IMAGE *LoadedImage = NULL;
    EFI_FILE_IO_INTERFACE *IOVolume = NULL;
    EFI_FILE_HANDLE Volume;
    EFI_GUID iGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &iGuid, (void **)&LoadedImage);
    uefi_call_wrapper(BS->HandleProtocol, 3, LoadedImage->DeviceHandle, &fsGuid, (VOID *)&IOVolume);
    uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
    return Volume;
}

EFI_FILE *LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Attempting to load %s\n", Path);
    EFI_FILE *LoadedFile;
    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);
    uefi_call_wrapper(Volume->Open, 5, Volume, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    return LoadedFile;
}
