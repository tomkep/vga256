#include "vga256.h"

int FillColor;

static const unsigned char FillStyle[12][8] =
{
  {                      /* No Fill                       */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  {                      /* Solid Fill                    */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  },
  {                      /* Line Fill                     */
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
  },
  {                      /* Lt Slash Fill                 */
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
  },
  {                      /* Slash Fill                    */
    0xE0, 0xC1, 0x83, 0x07, 0x0E, 0x1C, 0x38, 0x70
  },
  {                      /* Backslash Fill                */
    0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x83, 0xC1
  },
  {                      /* Lt Backslash Fill             */
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
  },
  {                      /* Hatch Fill                    */
    0xFF, 0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88
  },
  {                      /* XHatch Fill                   */
    0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81
  },
  {                      /* Interleave Fill               */
    0xCC, 0x33, 0xCC, 0x33, 0xCC, 0x33, 0xCC, 0x33
  },
  {                      /* Wide Dot Fill                 */
    0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x00
  },
  {                      /* Close Dot Fill                */
    0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00
  }
};

static unsigned char CurFillStyle[8];

void SetFillStyle(char Pat, unsigned char far *UserPat)
{
  int i;
  if(Pat == -1)
    for(i = 0; i < 8; i++)
      CurFillStyle[i] = UserPat[i];
  else
    for(i = 0; i < 8; i++)
      CurFillStyle[i] = FillStyle[Pat][i];
}

void PatBar(int x1, int y1, int x2, int y2)
{
  int x, y;
  if(x1 > x2)
    x = x1, x1 = x2, x2 = x;
  if(y1 > y2)
    y = y1, y1 = y2, y2 = y;
  for(y = y1; y <= y2; y++)
  {
    char c = CurFillStyle[y & 7];
    c = _crotl(c, x1 & 7);
    for(x = x1; x <= x2; x++)
    {
      c = _crotl(c, 1);
      PutPixel(x, y, (c & 1) ? FillColor : 0);
    }
  }
}
