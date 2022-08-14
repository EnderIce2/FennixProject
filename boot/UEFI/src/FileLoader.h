#pragma once
#include <efi.h>
#include <efilib.h>

EFI_FILE *LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
