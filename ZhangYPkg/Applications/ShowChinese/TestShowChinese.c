#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>
#include "ShowChinese.h"
#include "InitalProtocols.h"
#include "KeyBoard.h"

extern CHINESE_FONT_INFO myFontArray;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[];
extern CHINESE_LATTICE latticeArray[];

UINT8 *s_text = (UINT8 *)"你好";

INTN
EFIAPI
ShellAppMain(
    IN UINTN   Argc,
    IN CHAR16  **Argv
){
    EFI_STATUS status;
    //Print(L"TestShowChinese ShellAppMain is running\n");

    status = InitGolableProtocols();

    WaitKey();

    SwitchGraphicsMode(TRUE);
    
    SetBKG(&gColorTable[1]);

    Print(L"s_text = %d", *s_text);
    draw_string(s_text, 80, 60, &myFontArray, &gColorTable[0]);
    draw_string(s_text, 80, 100, &myFontArray, &gColorTable[0]);
    draw_string(s_text, 80, 120, &myFontArray, &gColorTable[0]);
    draw_string(s_text, 80, 140, &myFontArray, &gColorTable[0]);
    //Print(L"ZhangYPkg/Applications/ShowChinese/TestShowChinese.c Line : 34\n");
    WaitKey();
    return 0;
}