#include "vga256.h"

void SaveBitmap(int x1, int y1, void far *ptr)
{
  int x, y, x2, y2;
  x2 = x1 + *((int far *)ptr)++;
  y2 = y1 + *((int far *)ptr)++;
  for(y = y1; y <= y2; y++)
    for(x = x1; x <= x2; x++)
      *((char far *)ptr)++ = GetPixel(x, y);
}

void RestoreBitmap(int x1, int y1, char mode, void far *ptr)
{
  int x, y, x2, y2;
  x2 = x1 + *((int far *)ptr)++;
  y2 = y1 + *((int far *)ptr)++;
  for(y = y1; y <= y2; y++)
    for(x = x1; x <= x2; x++)
      switch(mode)
      {
        case CopyPut:
          PutPixel(x, y, *((char far *)ptr)++);
          break;
        case XorPut:
          XorPutPixel(x, y, *((char far *)ptr)++);
          break;
        case OrPut:
          OrPutPixel(x, y, *((char far *)ptr)++);
          break;
        case AndPut:
          AndPutPixel(x, y, *((char far *)ptr)++);
          break;
        case NotPut:
          NotPutPixel(x, y, *((char far *)ptr)++);
          break;
      }
}
