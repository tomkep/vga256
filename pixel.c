#include "vga256.h"

int GetPixel(int x, int y)
{
  outportw(0x3CE, (x & 3) << 8 | 4);
  return (*(VideoSeg + VideoOff + y * 80 + x / 4));
}

void PutPixel(int x, int y, int color)
{
  outportw(0x3CE, 3);
  outportw(0x3C4, 0x100 << (x & 3) | 2);
  _ES = (unsigned)VideoSeg;
  _BX = (unsigned)(VideoOff + y * 80 + x / 4);
  _AL = color;
  asm   xchg    es:[bx],al
}

void XorPutPixel(int x, int y, int color)
{
  outportw(0x3CE, 0x1803);
  outportw(0x3C4, 0x100 << (x & 3) | 2);
  _ES = (unsigned)VideoSeg;
  _BX = (unsigned)(VideoOff + y * 80 + x / 4);
  _AL = color;
  asm   xchg    es:[bx],al
}

void OrPutPixel(int x, int y, int color)
{
  outportw(0x3CE, 0x1003);
  outportw(0x3C4, 0x100 << (x & 3) | 2);
  _ES = (unsigned)VideoSeg;
  _BX = (unsigned)(VideoOff + y * 80 + x / 4);
  _AL = color;
  asm   xchg    es:[bx],al
}

void AndPutPixel(int x, int y, int color)
{
  outportw(0x3CE, 0x0803);
  outportw(0x3C4, 0x100 << (x & 3) | 2);
  _ES = (unsigned)VideoSeg;
  _BX = (unsigned)(VideoOff + y * 80 + x / 4);
  _AL = color;
  asm   xchg    es:[bx],al
}

void NotPutPixel(int x, int y, int color)
{
  outportw(0x3CE, 3);
  outportw(0x3C4, 0x100 << (x & 3) | 2);
  _ES = (unsigned)VideoSeg;
  _BX = (unsigned)(VideoOff + y * 80 + x / 4);
  _AL = ~color;
  asm   xchg    es:[bx],al
}
