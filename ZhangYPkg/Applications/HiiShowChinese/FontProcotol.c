#include "FontProcotol.h"

//AF2BF00C-C2D9-A741-59DB-DCB499342978
EFI_GUID gmySimpleFontPkgListGuid = { \
    0xAF2BF00C, 0xC2D9, 0xA741, {0x59, 0xDB, 0xDC, 0xB4, 0x99, 0x34, 0x29, 0x78 } \
};

extern EFI_NARROW_GLYPH              gSimpleFontNarrowGlyphData[];
extern EFI_WIDE_GLYPH              gSimpleFontWideGlyphData[];

/**
 * @reference: D:\edk2\MdeModulePkg\Universal\Console\GraphicsConsoleDxe\GraphicsConsole.c RegisterFontPackage
 * 
*/
EFI_STATUS
EFIAPI
CreatSimpleFontPkg(
    IN EFI_NARROW_GLYPH *NarrowGlyph, 
    IN UINT32 nNarrow, 
    IN EFI_WIDE_GLYPH *WideGlyph, 
    IN UINT32 nWide
){
    EFI_HII_SIMPLE_FONT_PACKAGE_HDR *simpleFontPkgHeader;
    EFI_HII_HANDLE mHiiHandle;
    UINT8 *package;
    UINT32 packageLen = sizeof(EFI_HII_SIMPLE_FONT_PACKAGE_HDR) + nNarrow + nWide + 4;
    UINT8 *Location;

    simpleFontPkgHeader->Header.Length = 
    

    package = (UINT8 *)AllocateZeroPool(packageLen);
    ASSERT (package != NULL);
    WriteUnaligned32((UINT32 *)package, packageLen);

      //
  // Add 4 bytes to the header for entire length for HiiAddPackages use only.
  //
  //    +--------------------------------+ <-- Package
  //    |                                |
  //    |    PackageLength(4 bytes)      |
  //    |                                |
  //    |--------------------------------| <-- SimplifiedFont
  //    |                                |
  //    |EFI_HII_SIMPLE_FONT_PACKAGE_HDR |
  //    |                                |
  //    |--------------------------------| <-- Location
  //    |                                |
  //    |     gSimpleFontNarrowGlyphData |
  //    |                                |
  //    +--------------------------------+ <-- Location + nNarrow
  //    |                                |
  //    |     gSimpleFontWideGlyphData   |
  //    |                                |
  //    +--------------------------------+
  


    simpleFontPkgHeader = (EFI_HII_SIMPLE_FONT_PACKAGE_HDR *)(package + 4);
    simpleFontPkgHeader->Header.Length        = (UINT32)(packageLen - 4);
    simpleFontPkgHeader->Header.Type          = EFI_HII_PACKAGE_SIMPLE_FONTS;
    simpleFontPkgHeader->NumberOfNarrowGlyphs = (UINT16)(nNarrow / sizeof (EFI_NARROW_GLYPH));
    simpleFontPkgHeader->NumberOfWideGlyphs = (UINT16)(nWide / sizeof (EFI_WIDE_GLYPH));
    Location = (UINT8 *)(&simpleFontPkgHeader->NumberOfWideGlyphs + 1);
    CopyMem (Location, gSimpleFontNarrowGlyphData, nNarrow);
    Location += nNarrow;
    CopyMem (Location, gSimpleFontWideGlyphData, nWide);
    mHiiHandle = HiiAddPackages (
                    &gmySimpleFontPkgListGuid,
                    NULL,
                    package,
                    NULL,
                    NULL
                    );
    ASSERT (mHiiHandle != NULL);
    FreePool(package);
    return EFI_SUCCESS;
}

//C4BDA449-A35B-D21F-0DF5-CBF5853AE57D
EFI_GUID gmyFontPkgListGuid = { \
    0xC4BDA449, 0xA35B, 0xD21F, {0x0D, 0xF5, 0xCB, 0xF5, 0x85, 0x3A, 0xE5, 0x7D } \
};

#define wfontWidth 16
#define wfontHight 19
CONST UINT16 wideGlyphBytes = wfontHight * wfontWidth / 8;
CONST UINT16 nrGlyphBytes = 19;

//添加 skip2 block 和 一个由 NarrowGlyph 构成的 EFI_HII_GIBT_GLYPHS_BLOCK
EFI_STATUS FillNarrowGLYPH(UINT8 *p, EFI_NARROW_GLYPH *NarrowGlyph, UINT32 SizeInBytes, CHAR16 Next, CHAR16 NrStart, CHAR16 NrCharNum)
{
    UINT8 *BitmapData = NULL;
    UINTN Length = 0;
    EFI_HII_GLYPH_INFO Cell = {8, 19, 0, 0, (INT16)8};//narrow glyph的 cell信息，
    EFI_HII_GIBT_GLYPHS_BLOCK *GlyphsBlock; //2019-6-7 11:59:05 luobing ammend

    // SKIP
    if (Next != NrStart)// 1 != 32 //不需要显示的字符包括 0x00 - 0x1F，将此部分用 EFI_HII_GIBT_SKIP2_BLOCK填充
    {
        EFI_HII_GIBT_SKIP2_BLOCK *FontSkip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)p;
        FontSkip2Block->Header.BlockType = (UINT8)EFI_HII_SIBT_SKIP2;
        FontSkip2Block->SkipCount = NrStart - Next; //跳过固定数量的字符，

        p = (UINT8 *)(FontSkip2Block + 1);
        Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }

    //    EFI_HII_GIBT_GLYPHS_BLOCK * GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);
    GlyphsBlock = (EFI_HII_GIBT_GLYPHS_BLOCK *)(p);//narrowglyph block
    GlyphsBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_GLYPHS;
    GlyphsBlock->Cell = Cell;
    GlyphsBlock->Count = NrCharNum;//一共 NrCharNum 个字符
    BitmapData = GlyphsBlock->BitmapData;
    {
        UINT16 i = 0, j = 0;
        for (i = 0; i < NrCharNum; i++)
        {
            for (j = 0; j < 19; j++)//
            {
                BitmapData[j] = NarrowGlyph[i].GlyphCol1[j]; //取的字形信息。
            }
            BitmapData += 19;
        }
    }
    Length += (sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) + 19 * NrCharNum - 1);
    return (EFI_STATUS)Length;
}


/***
  构建Font资源包，并向Hii数据库注册
  @param[in]  FontName      字体名字
  @param[in]  FontWidth     字体宽度
  @param[in]  FontHeight    字体高度
  @param[in]  NarrowGlyph   窄字符字模信息
  @param[in]  NrSizeInBytes 窄字符数组字节长
  @param[in]  NrStart       窄字符起始字符
  @param[in]  NrCharNum     窄字符数目
  @param[in]  WideGlyph     宽字符字模信息 
  @param[in]  SizeInBytes   宽字符数组字节长
  @param[in]  Start         宽字符起始字符  0x100 - 0x4dff 不需要显示
  @param[in]  CharNum       宽字符数目
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
  注意： Font资源包中，不存在窄字符和宽字符的概念，本函数主要是借用了SimpleFont的字模信息，
        构建了Font资源包而已。
***/
EFI_STATUS
EFIAPI
CreatFontPkg(
    IN CHAR8 *fontName,
    IN UINT16 fontWidth,
    IN UINT16 fontHeight,
    IN EFI_NARROW_GLYPH *narrowGlyph, 
    IN UINT32 nrSizeInBytes, 
    IN CHAR16 nrStart, 
    IN CHAR16 nrCharNum,
    IN EFI_WIDE_GLYPH *wideGlyph, 
    IN UINT32 sizeInBytes, 
    IN CHAR16 start, 
    IN CHAR16 charNum
){
    EFI_HII_FONT_PACKAGE_HDR *fontPkgHeader; //Font 专用的数据结构
    UINT32 packageLength;
    UINT8 *package, *pCurrent;
    UINTN blockLength = 0;
    CHAR16 nextChar = 0;
    EFI_GLYPH_GIBT_END_BLOCK *fontEndBlock;

    UINT16 fontNameLen = (UINT16)StrLen(fontName) * 2 + 2;



    packageLength = 4 + sizeof(EFI_HII_FONT_PACKAGE_HDR) + (fontNameLen /*Max Length of Font Name*/) +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) - 1 + charNum * wideGlyphBytes +
                    sizeof(EFI_HII_GIBT_SKIP2_BLOCK) +
                    sizeof(EFI_HII_GIBT_GLYPHS_BLOCK) - 1 + nrCharNum * nrGlyphBytes +
                    sizeof(EFI_GLYPH_GIBT_END_BLOCK);
    package = (UINT8 *)AllocateZeroPool(packageLength);


    WriteUnaligned32((UINT32 *)package, packageLength);
    fontPkgHeader->FontStyle = EFI_HII_FONT_STYLE_NORMAL;
    fontPkgHeader->Header.Type = EFI_HII_PACKAGE_FONTS;
    fontPkgHeader->Header.Length = (packageLength - 4);
    //cell
    fontPkgHeader->Cell.Width = fontWidth;
    fontPkgHeader->Cell.Height = fontHeight;
    fontPkgHeader->Cell.OffsetX = 0;
    fontPkgHeader->Cell.OffsetY = 0;
    fontPkgHeader->Cell.AdvanceX = (INT16)fontWidth;

    fontPkgHeader->HdrSize = sizeof(EFI_HII_FONT_PACKAGE_HDR) + fontNameLen;
    CopyMem((fontPkgHeader->FontFamily), fontName, fontNameLen);//以fontName 作为字体系列的名称
    fontPkgHeader->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + fontNameLen;

    pCurrent = 4 + sizeof(EFI_HII_FONT_PACKAGE_HDR) + (fontNameLen /*Max Length of Font Name*/);

    blockLength = FillNarrowGLYPH(pCurrent, narrowGlyph, nrSizeInBytes, 1, nrStart, nrCharNum);
    pCurrent += blockLength;
    nextChar = nrStart + nrCharNum;
    blockLength = FillWideGLYPH(pCurrent, wideGlyph, sizeInBytes, nextChar, start, charNum);
    pCurrent += blockLength;

    fontEndBlock = (EFI_GLYPH_GIBT_END_BLOCK *)(pCurrent);
    fontEndBlock->Header.BlockType = (UINT8)EFI_HII_GIBT_END;
    {
        EFI_HANDLE gFontHiiHandle = HiiAddPackages(
            &gmyFontPkgListGuid,
            NULL,
            package,
            NULL);
        (void)gFontHiiHandle;
        //ASSERT (gFontHiiHandle != NULL);
    }
    FreePool(package);
    return EFI_SUCCESS;
}


/***
  注册Font点阵库

  @param[in/out]  void
  @retval  EFI_SUCCESS         执行成功
  @retval  Other               发生错误
***/
EFI_STATUS LoadFont(void)
{
    EFI_STATUS Status = 0;
    EFI_HII_HANDLE *handles1 = 0; //for Font of Hii
    handles1 = HiiGetHiiHandles(&gmyFontPkgListGuid);
    if (handles1 == 0)
    {
        //extern CHAR16* FontName ;
        CHAR16 *FontName = (CHAR16 *)L"LUOBING"; //随便取个名字
        Status = CreateMyFontPkg(FontName, 16, 19,
                                 gSimpleFontNarrowGlyphData, gSimpleFontNarrowBytes, 32, (CHAR16)(nMyNarrowGlyphSize / sizeof(EFI_NARROW_GLYPH)),
                                 gSimpleFontWideGlyphData, gSimpleFontWideBytes, 0x4e00, (CHAR16)(nMyWideGlyphSize / sizeof(EFI_WIDE_GLYPH)));
        Print(L"LoadFont:CreateMyFontPkg= %r\n", Status);
    }
    else
    {
        //gST->ConOut->OutputString(gST->ConOut,L"execute CreateMyFontPkg()  handles1==1\n\r");
        FreePool(handles1);
        return EFI_LOAD_ERROR; //refer to EfiError.h
    }

    return Status;
}