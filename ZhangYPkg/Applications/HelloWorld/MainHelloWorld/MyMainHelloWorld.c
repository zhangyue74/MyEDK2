#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiBaseType.h>

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE *imageHandle, 
    IN EFI_SYSTEM_TABLE *systemTable
)
{
    Print(L"Main Hello World!\n");
    EFI_STATUS status;
    EFI_TIME currentTime;
    status = systemTable->BootServices->Stall(2000);
    systemTable->RuntimeServices->GetTime(
                                    &currentTime,
                                    NULL);
    Print(L"%d-%d-%d %02d:%02d:%02d\n", currentTime.Year, currentTime.Month, currentTime.Day, \
                                        currentTime.Hour, currentTime.Minute, currentTime.Second);
    return EFI_SUCCESS;
}