#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h> 
#include <Library/UefiLib.h>

INTN
EFIAPI
ShellAppMain (
  IN UINTN   Argc,
  IN CHAR16  **Argv
)
{
    Print(L"Shell Hello World!\n");

    EFI_STATUS status;
    EFI_TIME currentTime;
    status = gBS->Stall(2000);
    status = gRT->GetTime(
                    &currentTime,
                    NULL);
    Print(L"%d-%d-%d %02d:%02d:%02d\n", currentTime.Year, currentTime.Month, currentTime.Day, \
                                        currentTime.Hour, currentTime.Minute, currentTime.Second);
    return 0;
}