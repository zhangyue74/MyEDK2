#ifndef _INITAL_PROTOCOLS_H_
#define _INITAL_PROTOCOLS_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *g_graphicOutputProtocolHandle;
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *g_simpleTextInputExProtocolHandle;



EFI_STATUS
EFIAPI
InitGolableProtocols(
    VOID
);

#endif