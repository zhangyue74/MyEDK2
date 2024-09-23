#include "InitalProtocols.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *g_graphicOutputProtocolHandle;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *g_simpleTextInputExProtocolHandle;



/**
 * @funciton: 用来 初始化 GraphicOutputProtocol ，在 InitGolableProtocols 调用
 * @params: VOID,
 * @return: NULL,  
 */
EFI_STATUS
EFIAPI
InitGraphicOutputProtocol(
    VOID
){
    EFI_STATUS status;
    UINTN noHandles, i;
    EFI_HANDLE *graphicOutputHandleBuffer;
    

    status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiGraphicsOutputProtocolGuid,
                    NULL,
                    &noHandles,
                    &graphicOutputHandleBuffer
    );

    for (i = 0; i < noHandles; i++)
    {
        status = gBS->HandleProtocol(
                graphicOutputHandleBuffer[i],
                &gEfiGraphicsOutputProtocolGuid,
                (VOID**)(&g_graphicOutputProtocolHandle)
        );
        if(EFI_ERROR(status)){
            continue;
        }else{
            break;
        }
    }

    if(!g_graphicOutputProtocolHandle){
        Print(L"ZhangYPkg/Applications/ShowChinese/InintalProtocols.c LINE: %d ERROR!\n", __LINE__);
        return status;
    }
    
    return status;
}

/**
 * @funciton: 用来 初始化 SimpleTextInputExProtocol ，在 InitGolableProtocols 调用
 * @params: VOID,
 * @return: NULL,  
 */
EFI_STATUS
EFIAPI
InitSimpleTextInputExProtocol(
    VOID
){
    EFI_STATUS status;
    UINTN noHandles, i;
    EFI_HANDLE *simpleTextInputExHandleBuffer;
    

    status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiSimpleTextInputExProtocolGuid,
                    NULL,
                    &noHandles,
                    &simpleTextInputExHandleBuffer
    );

    for (i = 0; i < noHandles; i++)
    {
        status = gBS->HandleProtocol(
                simpleTextInputExHandleBuffer[i],
                &gEfiSimpleTextInputExProtocolGuid,
                (VOID**)(&g_simpleTextInputExProtocolHandle)
        );
        if(EFI_ERROR(status)){
            continue;
        }else{
            break;
        }
    }

    if(!g_simpleTextInputExProtocolHandle){
        Print(L"ZhangYPkg/Applications/ShowChinese/InintalProtocols.c LINE: %d ERROR!\n", __LINE__);
        return status;
    }
    
    return status;
}


/**
 * @funciton: 用来 初始化所有需要使用的 Protocols ，在主程序中最开始调用
 * @params: VOID,
 * @return: NULL,   主要是为两个全局变量协议 赋值，找到这两个Protocol的实例
 */
EFI_STATUS
EFIAPI
InitGolableProtocols(
    VOID
){
    EFI_STATUS status;

    status = InitGraphicOutputProtocol();
    if(EFI_ERROR(status)){
        return status;
    }

    status = InitSimpleTextInputExProtocol();
    if(EFI_ERROR(status)){
        return status;
    }
    return status;
}