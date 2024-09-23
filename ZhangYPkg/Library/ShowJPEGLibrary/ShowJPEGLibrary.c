#include "ShowJPEGLibrary.h"


RETURN_STATUS
EFIAPI
ShowLibraryConstructor(VOID){
    Print(L"MyLibConstructor is called!\n");
}

RETURN_STATUS
EFIAPI
ShowLibraryLibDestructor(VOID){
    Print(L"MyLibDestructor is called!\n");
}


VOID
EFIAPI
PutPixel(
    IN UINTN x,
    IN UINTN y,
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
){
    
    
}