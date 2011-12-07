#include "vga256.h"

static int BorderColor;
static int StackPtr;
static POINT Stack[256];
static int UpXLimit, UpXLimit1;

static int ScanDownForBorder(int x, int y)
{
  for( ; x >= vp.x1; x--)
    if(GetPixel(x, y) == BorderColor)
      break;
  return (x + 1);
}

static int ScanUpForBorder(int x, int y)
{
  for( ; x <= vp.x2; x++)
    if(GetPixel(x, y) == BorderColor)
      break;
  return (x - 1);
}

static int ScanUpForNotBorder(int x, int xmax, int y)
{
  for( ; x <= xmax; x++)
    if(GetPixel(x, y) != BorderColor)
      return (x);
  return (-1);
}

static int PopPoint(int x, int y, int DeltaY)
{
  int i;
  if(DeltaY < 0)
    x = ~x;
  for(i = 0; i < StackPtr; i++)
    if((Stack[i].x == x) && (Stack[i].y == y))
    {
      for(StackPtr--; i < StackPtr; i++)
        Stack[i] = Stack[i + 1];
      return (1);
    }
  return (0);
}

static void PushPoint(int x, int maxx, int y, int DeltaY)
{
  while((x = ScanUpForNotBorder(x, maxx, y)) >= 0)
  {
    Stack[StackPtr].x = DeltaY >= 0 ? x : ~x;
    Stack[StackPtr++].y = y;
    x = ScanUpForBorder(x, y) + 1;
  }
}

static void Scanner(int x, int y, int DeltaY)
{
  int ny, lx;
  do
  {
    ny = y + DeltaY;
    if((ny < vp.y1) || (ny > vp.y2))
      break;
    if(GetPixel(x, ny) != BorderColor)
      lx = ScanDownForBorder(x, ny);
    else
      if((lx = ScanUpForNotBorder(x, UpXLimit, ny)) < 0)
        break;
    UpXLimit1 = UpXLimit;
    UpXLimit = ScanUpForBorder(lx, ny);
    PatBar(lx, ny, UpXLimit, ny);
    PushPoint(lx, x - 1, y, -DeltaY);
    if(UpXLimit1 != UpXLimit)
    {
      int i, j, x1, x2, dy;
      if(UpXLimit1 <= UpXLimit)
      {
        x1 = UpXLimit;
        x2 = UpXLimit1;
        dy = DeltaY;
      }
      else
      {
        x1 = UpXLimit1;
        x2 = UpXLimit;
        dy = -DeltaY;
        y = ny;
      }
      if((j = ScanUpForBorder(i = x1, y)) > x1)
      {
        while((j = ScanUpForBorder(i = j, y + dy)) > i &&
              (j = ScanUpForBorder(i = j, y)) > i) ;
        PushPoint(x1 + 1, i, y + dy, dy);
      }
      PushPoint(x2 + 1, i, y, -dy);
    }
  } while(PopPoint(x = lx, y = ny, -DeltaY) == 0);
}

void FloodFill(int x, int y, unsigned char color)
{
  int DeltaY = -1;
  BorderColor = color;
  if(GetPixel(x, y) != BorderColor)
  {
    Stack[0].x = x = ScanDownForBorder(x, y);
    Stack[0].y = y;
    StackPtr = 1;
    UpXLimit = ScanUpForBorder(x, y);
    while(1)
    {
      Scanner(x, y, DeltaY);
      do
      {
        if(StackPtr == 0)
          return;
        x = Stack[--StackPtr].x;
        y = Stack[StackPtr].y;
        if(x >= 0)
          DeltaY = 1;
        else
        {
          DeltaY = -1;
          x = ~x;
        }
        UpXLimit1 = UpXLimit;
        UpXLimit = ScanUpForBorder(x, y);
        PatBar(x, y, UpXLimit, y);
      } while(PopPoint(x, y, -DeltaY) != 0);
    }
  }
}
