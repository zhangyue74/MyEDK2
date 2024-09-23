#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>
#include <../MyLibrary/MyLibrary.h>


INTN
EFIAPI
ShellAppMain (
  IN UINTN   Argc,
  IN CHAR16  **Argv
){
    if (Argc != 2)
    {
      Print(L"Usage <InputString>\n");
      return 0;
    }
    
    EFI_STATUS  status;
    Print(L"This is MyLibrary test function!\n");
    status = MyTestLibraryFunction(Argv[1]);

    return 0;
}