#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/SerialIo.h>
#include <Protocol/Rng.h>
#include <Library\UefiBootServicesTableLib.h>


EFI_STATUS
EFIAPI
ListProtocolMsg(
    IN EFI_GUID *protocolGuid,
    OUT VOID **inferface
);

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE *ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemHandle
){

    EFI_SERIAL_IO_PROTOCOL *serialProtocol;
    EFI_RNG_PROTOCOL *rngProtocol;
    Print(L"Action serialProtocol\n");
    ListProtocolMsg(&gEfiSerialIoProtocolGuid, (VOID **)(&serialProtocol));
    Print(L"Action rngProtocol\n");
    ListProtocolMsg(&gEfiRngProtocolGuid, (VOID **)(&rngProtocol));


    return EFI_SUCCESS;
}


/**
 * @function: Protocol 使用方法示例程序
 * @steps: 1.使用 LocateHandleBuffer 函数，得到指定 Protocol GUID 的所有设备句柄 EFI_SERIAL_IO_PROTOCOL
 *         2.通过 HandleProtocol 函数得到最先扫描的设备句柄的 Protocol 指针
 *         3.获取最先扫描设备句柄的 Protocol 信息，这是通过 OpenProtocolInformation 函数实现的
 *         4.调用 CloseProtocol 关闭 Protocol ，并回收被调用函数申请的内存
 * 
 * */
EFI_STATUS
EFIAPI
ListProtocolMsg(
    IN EFI_GUID *protocolGuid,
    OUT VOID **inferface
){
    EFI_STATUS status;
    UINTN noHandles;
    UINTN i, entryCount;
    EFI_HANDLE *buffer;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *informationEntry;
    status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    protocolGuid,
                    NULL,
                    &noHandles,
                    &buffer
    );
    if (EFI_ERROR(status))
    {
        Print(L"LocateHandleBuffer Function Error!\n");
        return status;
    }
    for (i = 0; i < noHandles; i++)
    {
        status = gBS->HandleProtocol(
                        buffer[i],
                        protocolGuid,
                        inferface);
        if(EFI_ERROR(status)){
            continue;
        }else{
            break;
        }
    }

    status = gBS->OpenProtocolInformation(
                    buffer[i],
                    protocolGuid,
                    &informationEntry,
                    &entryCount
    );
    if (EFI_ERROR(status))
    {
        Print(L"No Information!\n");
    }else{
        Print(L"EntryCount = %d\n", entryCount);
        status = gBS->CloseProtocol(
                        buffer[i],
                        protocolGuid,
                        informationEntry->AgentHandle,
                        informationEntry->ControllerHandle
        );
        if(informationEntry){
            gBS->FreePool(informationEntry);
        }

        if(buffer){
            gBS->FreePool(buffer);
        }
    }
    
    
    return status;

}