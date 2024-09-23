/**
 * @file: 演示如何使用 随机数协议
 * @version: 20240724 1.0
 * @steps: 1.使用 LocateHandleBuffer 函数 得到支持 GUID Protocol的所有设备
 *         2.使用 HandleProtocol 获得第一个扫描设备句柄的Protocol 实例
 *         3.通过 OpenProtocolInformation 得到 Protocol 信息
 *         4.通过 CLoseProtocol 关闭Protocol
 * @retval: NULL
 */

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/Rng.h>


EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE *imageHandle,
    IN EFI_SYSTEM_TABLE *systemTable 
){
    EFI_STATUS status;
    UINTN noHandle;
    EFI_HANDLE *handleBuffer;
    EFI_RNG_PROTOCOL *rngHandle;
    UINTN i, entryCount;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *informationEntry;

    status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiRngProtocolGuid,
                    NULL,
                    &noHandle,
                    &handleBuffer
    );

    if (status == EFI_INVALID_PARAMETER)
    {
        Print(L"Error of LocateHandleBuffer , \
                NoHandles or Buffer is NULL\n");
        return status;
    }else if(status == EFI_NOT_FOUND){
        Print(L"Error of LocateHandleBuffer , \
                No handles match the search.\n");
        return status;
    }else if(status == EFI_OUT_OF_RESOURCES){
        Print(L"Error of LocateHandleBuffer , \
                There is not enough pool memory to store the matching results.\n");
        return status;
    }

    for (i = 0; i < noHandle; i++)
    {
        status = gBS->HandleProtocol(
                        handleBuffer[i],
                        &gEfiRngProtocolGuid,
                        (VOID**)(&rngHandle)
        );

         if (status == EFI_UNSUPPORTED)
        {
            Print(L"Error of HandleProtocol , \
                    The device does not support the specified protocol.\n");
            continue;
        }else if(status == EFI_INVALID_PARAMETER){
            Print(L"Error of HandleProtocol , \
                    Handle or Protocol or Interface is NULL.\n");
            continue;
        }else{
            break;
        }
    }

    Print(L"handleBuffer[i] i = %d\n", i);

    status = gBS->OpenProtocolInformation(
                    handleBuffer[i],
                    &gEfiRngProtocolGuid,
                    &informationEntry,
                    &entryCount
    );

    if (status == EFI_NOT_FOUND)
    {
        Print(L"Error of OpenProtocolInformation , \
                Handle does not support the protocol specified by Protocol.\n");
        return status;
    }else if(status == EFI_OUT_OF_RESOURCES){
        Print(L"Error of OpenProtocolInformation , \
                There are not enough resources available to allocate EntryBuffer.\n");
        return status;
    }
    
    
    Print(L"informationEntry->OpenCount = : %d\n", informationEntry->OpenCount);
    
    status = gBS->CloseProtocol(
                    handleBuffer[i],
                    &gEfiRngProtocolGuid,
                    informationEntry->AgentHandle,
                    informationEntry->ControllerHandle
    );

    if (status == EFI_INVALID_PARAMETER)
    {
        Print(L"Error of CloseProtocol , Handle is NULL or AgentHandle is NULL.\n");
        return status;
    }
    gBS->FreePool(informationEntry);
    gBS->FreePool(handleBuffer);

    return status;
}