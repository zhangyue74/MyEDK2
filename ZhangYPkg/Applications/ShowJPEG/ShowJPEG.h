#ifndef _SHOW_JPEG_H_
#define _SHOW_JPEG_H_


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>
#include <Protocol/GraphicsOutput.h>

EFI_STATUS
EFIAPI
PutPixel(
    IN UINTN x,
    IN UINTN y,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
);

EFI_STATUS
EFIAPI
GetPixel(
    IN UINTN x,
    IN UINTN y,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
);



#endif