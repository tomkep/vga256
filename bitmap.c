#include "vga256.h"

void SaveBitmap(int x1, int y1, Bitmap_t far *p)
{
  int x, y, x2 = x1 + p->dx, y2 = y1 + p->dy;
  unsigned char far *pp = p->bitmap;
  for(y = y1; y <= y2; ++y)
    for(x = x1; x <= x2; ++x)
      *pp++ = GetPixel(x, y);
}

void RestoreBitmap(int x1, int y1, char mode, Bitmap_t far *p)
{
  static PutPixelFunc_t putPixelFuncsTbl[] =
  {
    PutPixel,
    XorPutPixel,
    OrPutPixel,
    AndPutPixel,
    NotPutPixel
  };
  PutPixelFunc_t putPixelFunc = mode < 5 ? putPixelFuncsTbl[mode] : PutPixel;
  int x, y, x2 = x1 + p->dx, y2 = y1 + p->dy;
  unsigned char far *pp = p->bitmap;
  for(y = y1; y <= y2; ++y)
    for(x = x1; x <= x2; ++x)
      putPixelFunc(x, y, *pp++);
}
