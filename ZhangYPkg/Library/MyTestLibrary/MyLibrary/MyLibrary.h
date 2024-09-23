#ifndef _MY_LIBRARY_H_
#define _MY_LIBRARY_H_


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>


EFI_STATUS
EFIAPI
MyTestLibraryFunction(
     IN  CHAR16   *inputString
);


#endif