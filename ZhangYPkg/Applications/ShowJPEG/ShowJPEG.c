/**
 * @file: 构建绘制 JPEG 的函数
 * @version: 20240724 1.0
 * @retval: NULL
 */
#include "ShowJPEG.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *gEfiGraphicsOutputProtocol;



/**
 * @function: 赋值全局 EFI_GRAPHICS_OUTPUT_PROTOCOL 的实例 gEfiGraphicsOutputProtocol
 * @version: 20240724 1.0
 * @steps: 1.使用 LocateHandleBuffer 函数 得到支持 GUID Protocol的所有设备
 *         2.使用 HandleProtocol 获得第一个扫描设备句柄的Protocol 实例
 * @retval: EFI_SUCCESS         gEfiGraphicsOutputProtocol 实例已经成功赋值
 *          EFI_UNSUPPORTED     没有正常找到 handle 的 protocol
 */
EFI_STATUS
EFIAPI
GetGraphicOutputProtocol(
    VOID
){
    EFI_STATUS status;
    UINTN noHandle, i;
    EFI_HANDLE *handleBuffer;
    

    status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiGraphicsOutputProtocol,
                    NULL,
                    &noHandle,
                    &handleBuffer
    );

    if(EFI_ERROR(status)){
        Print(L"LocateHandleBuffer error\n");
        return status;
    }

    for (i = 0; i < noHandle; i++)
    {
        status = gBS->HandleProtocol(
                        handleBuffer[i],
                        &gEfiGraphicsOutputProtocol,
                        (VOID**)(&gEfiGraphicsOutputProtocol)
        );
        if (EFI_ERROR(status))
        {
            continue;
        }else{
            break;
        }
    }
    if (gEfiGraphicsOutputProtocol)
    {
        Print(L"HandleProtocol error\n");
        return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
}
    

/**
 * @function: 在 x y 像素点上绘制某种颜色
 * @version: 20240724 1.0
 * @params:
 *           IN UINTN x,         x位置
*            IN UINTN y,         y位置
*            IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color     颜色
 * @retval: EFI_SUCCESS         绘制成功 
 */
EFI_STATUS
EFIAPI
PutPixel(
    IN UINTN x,
    IN UINTN y,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
){
    EFI_STATUS status;
    status = gEfiGraphicsOutputProtocol->Blt(
                                    gEfiGraphicsOutputProtocol,
                                    color,
                                    EfiBltVideoFill,
                                    0, 0,
                                    x, y,
                                    1, 1,
                                    0 //Not used for EfiBltVideoFill 
    );
    return status;
}

/**
 * @function: 在 x y 像素点上得到颜色
 * @version: 20240724 1.0
 * @params:
 *           IN UINTN x,         x位置
 *           IN UINTN y,         y位置
 *           OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color     用于存放颜色
 * @retval: EFI_SUCCESS         读取成功
 */
EFI_STATUS
EFIAPI
GetPixel(
    IN UINTN x,
    IN UINTN y,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
){
    EFI_STATUS status;
    status = gEfiGraphicsOutputProtocol->Blt(
                                    gEfiGraphicsOutputProtocol,
                                    color,
                                    EfiBltVideoToBltBuffer,
                                    x, y,
                                    0, 0,
                                    1, 1,
                                    0 //如果使用的Delta为零，则整个BltBuffer都在被操作
    );
    return status;
}


/**
 * @function: 画线
 * @version: 20240724 1.0
 * @params:
 *               IN UINTN startX,   画线起点x坐标
                IN UINTN starty,
                IN UINTN endX,      画线终点x坐标
                IN UINTN endY,
                OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color    画线颜色
 * @retval: EFI_SUCCESS         画线成功
 */
EFI_STATUS
EFIAPI
Line(
    IN UINTN startX,
    IN UINTN starty,
    IN UINTN endX,
    IN UINTN endY,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
){
    EFI_STATUS status;
    
    return status;
}

/**
 * @function: 画直线
 * @version: 20240724 1.0
 * @params:
 *               IN UINTN startX,   画线起点x坐标
                IN UINTN starty,
                IN UINTN endX,      画线终点x坐标
                IN UINTN endY,
                OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color    画线颜色
 * @retval: EFI_SUCCESS         画线成功
 */
EFI_STATUS
EFIAPI
HLine(
    IN UINTN startX,
    IN UINTN starty,
    IN UINTN endX,
    IN UINTN endY,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
){
    EFI_STATUS status;
    
    return status;
}