#include "MyLibrary.h"


RETURN_STATUS
EFIAPI
MyLibConstructor(VOID){
    Print(L"MyLibConstructor is called!\n");
}

RETURN_STATUS
EFIAPI
MyLibDestructor(VOID){
    Print(L"MyLibDestructor is called!\n");
}

EFI_STATUS
EFIAPI
MyTestLibraryFunction(
    IN  CHAR16   *inputString
){
    Print(L"MyTestLibraryFunction is called! inputNumber =  %s\n", inputString);
    return EFI_SUCCESS;
}