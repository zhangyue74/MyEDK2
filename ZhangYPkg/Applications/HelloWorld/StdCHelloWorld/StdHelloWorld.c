#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h> 
#include <Library/UefiLib.h>
#include  <stdio.h>

/**
 * 编译失败 build.py...
d:\edk2\ZhangYPkg\ZhangYPkg.dsc(...): error 4000: Instance of library class [LibCType] is not found
        in [d:\edk2-libc\StdLib\LibC\Stdio\Stdio.inf] [X64]
        consumed by module [d:\edk2\ZhangYPkg\Applications\HelloWorld\StdCHelloWorld\StdHelloWorld.inf]
 */


int
main(
    int Argc,
    char **Argv
)
{
    printf("Hello,this is Entry of main!\n");
    EFI_STATUS status;
    EFI_TIME currentTime;
    status = gBS->Stall(2000);
    status = gRT->GetTime(
                    &currentTime,
                    NULL);
    Print(L"%d-%d-%d %02d:%02d:%02d\n", currentTime.Year, currentTime.Month, currentTime.Day, \
                                        currentTime.Hour, currentTime.Minute, currentTime.Second);
    return 1;
}