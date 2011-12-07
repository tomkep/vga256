#include "vga256.h"

static int FontXSize;
static int FontYSize;
static int FontNo;
static int FontOrient;
static int FontXMult;
static int FontYMult;
static int FontXMarg;
static int FontYMarg;
static int FontBaseXSize = 8;
static int FontBaseYSize = 8;

unsigned char far *Font8x8Lo;
unsigned char far *Font8x8Hi;

POINT SetTextStyle(unsigned char NewFontNo,
                   unsigned char NewFontOrient, int XSize, int YSize)
{
  POINT r;
  FontNo = NewFontNo;
  FontOrient = NewFontOrient;
  FontXSize = XSize + 4 & 0xF8u;
  FontYSize = YSize + 4 & 0xF8u;
  FontXMult = FontXSize / 8;
  FontYMult = FontYSize / 8;
  FontXMarg = FontXSize - 1;
  FontYMarg = FontYSize - 1;
  r.x = FontXSize;
  r.y = FontYSize;
  return (r);
}

#pragma argsused
POINT GetTextSize(char far *string, int len)
{
  POINT RetVal;
  RetVal.x = FontXSize * len;
  RetVal.y = FontYSize;
  return (RetVal);
}

static unsigned char far *CharImage(int Code)
{
  static unsigned char Buffer[8];
  unsigned char far *ptr;
  int i, j;
  ptr = Code >= 128 ? Font8x8Hi : Font8x8Lo;
  if(Code >= 128)
    Code -= 128;
  ptr += Code * FontBaseYSize;
  if(FontOrient != fntNormalOrient)
    for(i = 0; i < 8; i++)
      Buffer[i] = 0;
  switch(FontOrient)
  {
    case fntNormalOrient:
      return (ptr);
    case fntVerticalOrient:
      for(i = 0; i < 8; i++)    /* byte */
        for(j = 0; j < 8; j++)  /* bit  */
          if(ptr[i] & 1 << j)
            Buffer[j] |= 0x80u >> i;
      break;
    case fntDownOrient:
      for(i = 0; i < 8; i++)    /* byte */
        for(j = 0; j < 8; j++)  /* bit  */
          if(ptr[i] & 1 << j)
            Buffer[7 - i] |= 0x80u >> j;
      break;
    case fntVerticalDownOrient:
      for(i = 0; i < 8; i++)    /* byte */
        for(j = 0; j < 8; j++)  /* bit  */
          if(ptr[i] & 1 << j)
            Buffer[7 - j] |= 1 << i;
      break;
  }
  return (Buffer);
}

static void DrawLetter(int x, int y, unsigned char znak)
{
  int i, j, k, l, xr;
  unsigned char far *table = CharImage(znak);
  unsigned char a;
  if(x >= vp.x1 && x <= vp.x2 - FontXMarg &&
     y >= vp.y1 && y <= vp.y2 - FontYMarg)
    for(i = 0; i < FontBaseYSize; i++)
    {
      a = table[i];
      for(j = 0; j < FontYMult; j++, y++)
      {
        xr = x;
        for(k = 0; k < FontBaseXSize; k++)
        {
          a = _crotl(a, 1);
          if(a & 1)
            for(l = 0; l < FontXMult; l++, xr++)
              PutPixel(xr, y, DrawColor);
          else
            xr += FontXMult;
        }
      }
    }
}

void Text(char far *string, int len)
{
  switch(FontOrient)
  {
    case fntNormalOrient:
      for(; len != 0; len--, string++, cp.x += FontXSize)
        DrawLetter(cp.x, cp.y, *string);
      break;
    case fntVerticalOrient:
      string += len - 1;
      for(; len != 0; len--, string--, cp.y += FontYSize)
        DrawLetter(cp.x, cp.y, *string);
      break;
    case fntDownOrient:
      string += len - 1;
      for(; len != 0; len--, string--, cp.x += FontXSize)
        DrawLetter(cp.x, cp.y, *string);
      break;
    case fntVerticalDownOrient:
      for(; len != 0; len--, string++, cp.y += FontYSize)
        DrawLetter(cp.x, cp.y, *string);
      break;
  }
}
