#include <stdlib.h>
#include "vga256.h"

static const int LineStyle[4] =
{
  0xFFFF,                /* Solid Line Style              */
  0xCCCC,                /* Dotted Line                   */
  0xFC78,                /* Center Line                   */
  0xF8F8                 /* Dashed Line                   */
};

static int CurLineStyle;
static int CurLineWidth;

void SetLineStyle(unsigned char Pat, int UserPat, int LineWidth)
{
  CurLineStyle = Pat == 4 ? UserPat : LineStyle[Pat];
  CurLineWidth = LineWidth;
}

void Vect(int x1, int y1, int x2, int y2)
{
  int deltaX = abs(x2 - x1),
      deltaY = abs(y2 - y1),
      stepX = (x1 > x2) ? -1 : 1,
      stepY = (y1 > y2) ? -1 : 1,
      i = CurLineStyle,
      c;
  if(deltaX > deltaY)
  {
    c = deltaX / 2;
    while(1)
    {
      i = _rotl(i, 1);
      if(i & 1)
        switch (WriteMode)
        {
          case CopyPut:
            PutPixel(x1, y1, DrawColor);
            break;
          case XorPut:
            XorPutPixel(x1, y1, DrawColor);
            break;
        }
      if(x1 == x2)
        break;
      x1 += stepX;
      c += deltaY;
      if(c >= deltaX)
      {
        c -= deltaX;
        y1 += stepY;
      }
    }
  }
  else
  {
    c = deltaY / 2;
    while(1)
    {
      i = _rotl(i, 1);
      if(i & 1)
        switch (WriteMode)
        {
          case CopyPut:
            PutPixel(x1, y1, DrawColor);
            break;
          case XorPut:
            XorPutPixel(x1, y1, DrawColor);
            break;
        }
      if(y1 == y2)
        break;
      y1 += stepY;
      c += deltaX;
      if(c >= deltaY)
      {
        c -= deltaY;
        x1 += stepX;
      }
    }
  }
}
