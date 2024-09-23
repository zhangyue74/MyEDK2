#include "ShowChinese.h"
#include "InitalProtocols.h"


/**
 * @prarms: 该数组是一个字典 ， 通过 utf-8 对汉字进行检索，最后得到 _utf-8_bitmap_bytes 字模信息
 */
CHINESE_LATTICE latticeArray[] = {
    {14990752, 32, _14990752_bitmap_bytes},
    {15050173, 32, _15050173_bitmap_bytes}
};

/**
 * @prarms: 该数组用于初始化 字模库的全局信息。
 */
CHINESE_FONT_INFO myFontArray = {
    32,
    2,
    latticeArray
};

/**
 * @prarms: EFI_GRAPHICS_OUTPUT_BLT_PIXEL 结构提用 32 bit 描述颜色信息，这里用于存常见颜色，
 */
EFI_GRAPHICS_OUTPUT_BLT_PIXEL gColorTable[]={
  {0,    0,    0,    0},        /*  BLACK */\
  {255,  255,  255,  0},        /*  WHITE */\
  {200,  208,  212,  0},        /*  LIGHT GRAY  */\
  {167,  95, 18, 0},        /*  DEEP BLUE */\
  {64, 64, 64, 0},        /*  DARK BLACK  */\
  {128,  128,  128,  0},        /*  LIGHT BLACK */\
  {0,    255,  255,  0},        /*  YELLOW  */\
  {0,    153,  0,    0},        /*  GREEN */\
  {0,    0,    153,  0},        /*  RED   */\
  {192,  0,    0,    0},        /* BLUE   */  
};

/**
 * @function: 通过utf-8编码 获得 对应的字模信息， 字模信息与utf-8对应，
 *              存放在 latticeArray 中，需要按utf-8编码升序排列
 * @version: 20240725 1.0
 * @params:
 *                   CHINESE_FONT_INFO   *fontInfo,     //.h文件中的 myFontArray
                     UINT32              utf8Code       //汉字的 utf-8 编码
 * @retval: EFI_SUCCESS         画线成功
 */
CHINESE_LATTICE*
GetChineseLattice(
    IN CHINESE_FONT_INFO   *fontInfo,
    IN UINT32              utf8Code
){
    INT32 head, tail = fontInfo->count-1;
    INT32 middle = (head + tail)/2;

    while(head <= tail){
        if (fontInfo->latticeArray[middle].utf8Code < utf8Code)
        {
            head = middle + 1;
        }else if(fontInfo->latticeArray[middle].utf8Code > utf8Code){
            tail = middle - 1;
        }else{
            return &(fontInfo->latticeArray[middle]);
        }
        middle = (head + tail)/2;
        
    }
    Print(L"2\n");
    return CANT_FIND_CHINESE_LATTICE;
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
    status = g_graphicOutputProtocolHandle->Blt(
                                    g_graphicOutputProtocolHandle,
                                    color,
                                    EfiBltVideoFill,
                                    0, 0,
                                    x, y,
                                    1, 1,
                                    0 //Not used for EfiBltVideoFill 
    );
    Print(L"2222\n");
    return status;
}


/**
 * @function: 绘制汉字
 * @version: 20240725 1.0
 * @params:
 *          UINTN
 * @retval: SHOW_SUCCESS         显示成功
 *          SHOW_ERROR           显示失败
 */
SHOW_RETURN_STATUS
DrawChineseLattice(
    IN  UINTN   x,
    IN  UINTN   y,
    IN  UINT8   height,
    IN  UINT8   width,
    IN  UINT8   *p_Data,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *color
){
    UINT8 blkValue = *p_Data++;
    UINT8 blkCnt = *p_Data++;
    UINT8 x_, y_;

    //x_ 和 y_ 是两个轴上的偏移量
    for (y_ = 0; y_ < height; y_++)
    {
        for (x_ = 0; x_ < width; x_++)
        {
            if (blkValue)//等于 1 绘制该像素
            {
                PutPixel((x + (UINTN)x_), (y + (UINTN)y_), color);
            }
            if (--blkCnt == 0)
            {
                blkValue = *p_Data++;
                blkCnt = *p_Data++;
            }
            
        }
    }
    //Print(L"6\n");
    return SHOW_SUCCESS;
}


/**
 * @function: 绘制单个汉字
 * @version: 20240725 1.0
 * @params:
 *          UINTN
 * @retval: SHOW_SUCCESS         显示成功
 *          SHOW_ERROR           显示失败
 */
UINT8
DrawSingleChar(
    IN  UINTN   x,
    IN  UINTN   y,
    IN  UINT32  utf8Code,
    IN  CHINESE_FONT_INFO   *fontInfo,  
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *color
){
    CHINESE_LATTICE *p_lattice;
    if (fontInfo)
    {
        p_lattice = GetChineseLattice(fontInfo, utf8Code);
        if(p_lattice == NULL){
            Print(L"4\n");
            return 0;
        }
        //Print(L"3\n");
        if(p_lattice){
            DrawChineseLattice(x, y, fontInfo->height, p_lattice->width, p_lattice->pixelGrayArray, \
                                color);
            //Print(L"1111\n");  
            return p_lattice->width;           
        }
    }
    Print(L"5\n");
    return 0;
}

/**
 * @function: 绘制单个汉字
 * @version: 20240725 1.0
 * @params:
 *          UINTN
 * @retval: SHOW_SUCCESS         显示成功
 *          SHOW_ERROR           显示失败
 */
VOID draw_string(
    IN  UINT8 *str, 
    IN  UINTN x, 
    IN  UINTN y, 
    IN  CHINESE_FONT_INFO *font, 
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *font_color
)
{
  UINT8 temp;
  UINT8 *ptr;
  // UINT32 i;
  UINT32 char_utf8;
  UINTN cur_x;
  ptr = (UINT8 *)str;
  cur_x = x;
  while('\0' != *ptr)
  {
    //1 get utf-8 code from str
    temp = ptr[0];
    if(temp > 128) //汉字
    {
      char_utf8 = (((UINT32)ptr[0]) << 16) + (((UINT32)ptr[1]) << 8) + (UINT32)ptr[2];
	    // char_utf8 = (((UINT32)ptr[0]) * 256 *256) + (((UINT32)ptr[1]) *256) + (UINT32)ptr[2];
      ptr += 3;
    }
    else
    {
      char_utf8 = (UINT32)ptr[0];
      ptr += 1;
    }
    //2 display a char
    cur_x += (UINTN)DrawSingleChar(cur_x, y, char_utf8, font, font_color);
    //Print(L"1\n");
  }
}


/**
 * @function: 打开/关闭 光标可见性
 * @version: 20240725 1.0
 * @params: BOOLEAN flag TRUE 关闭可见性
 *                       FALSE 打开可见性
 * @retval: EFI_SUCCESS         函数执行成功
 */
EFI_STATUS SwitchGraphicsMode(BOOLEAN flag)
{
	EFI_STATUS                         Status;
	if(flag)
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, FALSE); //关闭光标可见性
	}
	else
	{
		Status=gST->ConOut->EnableCursor (gST->ConOut, TRUE);
	}
	return Status;
}

UINT32 SYGraphicsMode;
UINT32 OldGraphicsMode;
UINT32 SYPixPerScanLine;


/**
 * @function: 绘制矩形方框
 * @version: 20240725 1.0
 * @params:     IN UINTN x1,        对角线两点的x轴坐标
                IN UINTN y1,
                IN UINTN x2,
                IN UINTN y2,
                IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color     绘制的举行颜色
 * @retval: EFI_SUCCESS         函数执行成功 
 */
VOID  rectblock(
    IN UINTN x1,
    IN UINTN y1,
    IN UINTN x2,
    IN UINTN y2,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
  UINTN minx,miny,maxx,maxy;
  
  minx=(x1<x2)?x1:x2;
  miny=(y1<y2)?y1:y2;
  maxx=(x1>x2)?x1:x2;
  maxy=(y1>y2)?y1:y2;
  
  g_graphicOutputProtocolHandle->Blt (g_graphicOutputProtocolHandle,color,EfiBltVideoFill,0,0,minx,miny,(maxx-minx+1),(maxy-miny+1),0);   
}

/**
 * @function: 获得支持的显示模式
 * @version: 20240725 1.0
 * @params:     IN	UINT32 HorRes,	//水平
                IN 	UINT32 VerRes,	//垂直
                OUT UINT32 *ModeNum,
                OUT UINT32 *CurrentModeNum,
                OUT UINT32 *PixelsPerScanLine
 * @retval: EFI_SUCCESS         函数执行成功 
 */
EFI_STATUS GetGraphicModeNumber(
	IN	UINT32 HorRes,	//水平
	IN 	UINT32 VerRes,	//垂直
	OUT UINT32 *ModeNum,
	OUT UINT32 *CurrentModeNum,
	OUT UINT32 *PixelsPerScanLine
)
{
	EFI_STATUS Status;
	UINT32 ModeIndex;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;

	*CurrentModeNum=g_graphicOutputProtocolHandle->Mode->Mode;	//store current mode 
	for(ModeIndex=0;ModeIndex<g_graphicOutputProtocolHandle->Mode->MaxMode;ModeIndex++)
	{
		Status=g_graphicOutputProtocolHandle->QueryMode(g_graphicOutputProtocolHandle,ModeIndex,&SizeOfInfo,&Info);
		if(!EFI_ERROR(Status))
		{
			if((Info->HorizontalResolution == HorRes) && (Info->VerticalResolution == VerRes))
			{
				*ModeNum=ModeIndex;
				return Status;
			}
		}
	}
	return EFI_NOT_FOUND;
}


/**
 * @function: 设置背景颜色
 * @version: 20240725 1.0
 * @params:     EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color
 * @retval: EFI_SUCCESS         函数执行成功 
 */
VOID SetBKG(IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *color)
{
	GetGraphicModeNumber(SY_SCREEN_WIDTH,SY_SCREEN_HEIGHT,&SYGraphicsMode,&OldGraphicsMode,&SYPixPerScanLine);
    g_graphicOutputProtocolHandle->SetMode(g_graphicOutputProtocolHandle,SYGraphicsMode);
    rectblock(0,0,SY_SCREEN_WIDTH-1,SY_SCREEN_HEIGHT-1,color);
}