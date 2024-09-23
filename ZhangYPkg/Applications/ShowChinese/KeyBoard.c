#include "InitalProtocols.h"
#include "KeyBoard.h"


//Function Name: WaitKey
//Input: none
//Output: none
//Description: Wait any key press
VOID WaitKey(VOID)
{
  EFI_INPUT_KEY     Key;
  UINTN Index;

  // flush keyboard buffer
  while ( gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS );
  gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
  return;
}