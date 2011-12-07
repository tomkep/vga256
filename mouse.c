#include "vga256.h"

static void interrupt (*OldInt33)(void);

static int fDraw;

static unsigned char near *MouseVideoOff;
static int CRTPage;

static struct
{
  int x, y;
  int dx, dy;
  char SavedArea[256];
} OldBackground;

static struct
{
  int x1, y1, x2, y2;
} Trap;

typedef struct
{
  int HotSpotX, HotSpotY;
  unsigned CursorTbl[2][16];
} CursorData;

static CursorData Cursor;
static CursorData ArrowCursor =
{
  0, 0,
  {
    {
      0x9FFF, 0x8FFF, 0x87FF, 0x83FF, 0x81FF, 0x80FF, 0x807F, 0x803F,
      0x801F, 0x800F, 0x80FF, 0x887F, 0x987F, 0xFC3F, 0xFC3F, 0xFE3F
    },
    {
      0x0000, 0x2000, 0x3000, 0x3800, 0x3C00, 0x3E00, 0x3F00, 0x3F80,
      0x3FC0, 0x3E00, 0x3600, 0x2300, 0x0300, 0x0180, 0x0180, 0x0000
    }
  }
};

static void DrawCursor(int x, int y)
{
  unsigned i, k, l;
  int x1, xmax, ymax;
  unsigned char near *m;
  if((x -= Cursor.HotSpotX) < 0)
  {
    OldBackground.dx = 15 + x;
    l = 0x8000u >> -x;
    x = 0;
  }
  else
  {
    OldBackground.dx = status[ModeNo].xres - x < 15 ? status[ModeNo].xres - x : 15;
    l = 0x8000u;
  }
  if(OldBackground.dx >= 0)
  {
    if((y -= Cursor.HotSpotY) < 0)
    {
      OldBackground.dy = 15 + y;
      i = -y;
      y = 0;
    }
    else
    {
      OldBackground.dy = status[ModeNo].yres - y < 15 ? status[ModeNo].yres - y : 15;
      i = 0;
    }
    if(OldBackground.dy >= 0)
    {
      OldBackground.x = x;
      OldBackground.y = y;
      xmax = x + OldBackground.dx;
      ymax = y + OldBackground.dy;
      m = VideoOff;
      VideoOff = MouseVideoOff;
      SaveBitmap(x, y, &OldBackground.dx);
      for(; y <= ymax; y++, i++)
        for(x1 = x, k = l; x1 <= xmax; k >>= 1, x1++)
          if(!(Cursor.CursorTbl[0][i] & k))
            PutPixel(x1, y, Cursor.CursorTbl[1][i] & k ? 15 : 0);
          else
            if(Cursor.CursorTbl[1][i] & k)
              XorPutPixel(x1, y, 15);
      VideoOff = m;
    }
    else
      OldBackground.y = -1;
  }
  else
    OldBackground.x = -1;
}

static void RestoreBackground(void)
{
  if(OldBackground.x >= 0 && OldBackground.y >= 0)
  {
    unsigned char near *i = VideoOff;
    VideoOff = MouseVideoOff;
    RestoreBitmap(OldBackground.x, OldBackground.y, CopyPut, &OldBackground.dx);
    VideoOff = i;
  }
}

static void huge (*UserRoutine)(void) = 0;
static unsigned UserMask;

#pragma saveregs
static void huge IntRoutine(void)
{
  int x = _CX, y = _DX;
  asm   test    ax,UserMask
  asm   jz      label_1
  asm   push    ax
  asm   mov     ax,word ptr UserRoutine
  asm   or      ax,word ptr UserRoutine+2
  asm   pop     ax
  asm   jz      label_1
  (*UserRoutine)();
  label_1:
  if(fDraw >= 0)
  {
    disable();
    if(x < Trap.x1 || x > Trap.x2 || y < Trap.y1 || y > Trap.y2)
    {
      RestoreBackground();
      DrawCursor(x, y);
    }
    else
    {
      fDraw = -1;
      RestoreBackground();
    }
    enable();
  }
}

#pragma argsused
static void interrupt Int33h(unsigned bp, unsigned di, unsigned si, unsigned ds,
                      unsigned es, unsigned dx, unsigned cx, unsigned bx,
                      unsigned ax, unsigned ip, unsigned cs, unsigned flags)
{
  int i, j;
  switch (ax)
  {
    case 0:                             // initialize
      MouseVideoOff = 0;
      CRTPage = 0;
      Cursor = ArrowCursor;
      UserMask = 0;
      fDraw = -1;
      *((unsigned char _seg *)Sel0040 + 0x0049) = 0x12;
      _AX = 0;
      (*OldInt33)();
      ax = _AX;
      _AX = 2;
      (*OldInt33)();
      _CX = 0;
      _DX = status[ModeNo].xres;
      _AX = 7;
      (*OldInt33)();
      _CX = 0;
      _DX = status[ModeNo].yres;
      _AX = 8;
      (*OldInt33)();
      _CX = status[ModeNo].xres / 2;
      _DX = status[ModeNo].yres / 2;
      _AX = 4;
      (*OldInt33)();
      _CX = 0x7F;
      _DX = FP_OFF(IntRoutine);
      _ES = _CS;
      _AX = 12;
      (*OldInt33)();
      break;
    case 1:                             // show cursor
      if(fDraw != 0)
      {
        if(fDraw == -1)
        {
          Trap.x1 = Trap.y1 = Trap.x2 = Trap.y2 = -1;
          _AX = 3;
          (*OldInt33)();
          DrawCursor(_CX, _DX);
        }
        fDraw++;
      }
      break;
    case 2:                             // hide cursor
      if(--fDraw == -1)
        RestoreBackground();
      break;
    case 4:                             // set position
      if(fDraw >= 0)
      {
        RestoreBackground();
        DrawCursor(cx, dx);
      }
      _chain_intr(OldInt33);
      break;
    case 9:                             // define cursor
      Cursor.HotSpotX = bx;
      Cursor.HotSpotY = cx;
      for(i = 0; i < 32; i++)
        ((unsigned *)Cursor.CursorTbl)[i] = ((unsigned far *)MK_FP(es, dx))[i];
      if(fDraw >= 0)
      {
        RestoreBackground();
        _AX = 3;
        (*OldInt33)();
        DrawCursor(_CX, _DX);
      }
      break;
    case 12:                            // set user's routine
      UserMask = cx;
      UserRoutine = (void huge (*)(void))MK_FP(es, dx);
      break;
    case 16:                            // set trap
      Trap.x1 = cx;
      Trap.y1 = dx;
      Trap.x2 = si;
      Trap.y2 = di;
      break;
    case 20:                            // change users's routine
      i = UserMask;
      UserMask = cx;
      cx = i;
      i = es;
      j = dx;
      es = FP_SEG(UserRoutine);
      dx = FP_OFF(UserRoutine);
      UserRoutine = (void huge (*)(void))MK_FP(i, j);
      break;
    case 29:                            // set cursor page
      CRTPage = bx % (ModeNo == 0 ? 4 : 2);
      if(fDraw >= 0)
        RestoreBackground();
      MouseVideoOff = (unsigned char near *)(CRTPage << (ModeNo == 0 ? 14 : 15));
      if(fDraw >= 0)
        DrawCursor(OldBackground.x, OldBackground.y);
      break;
    case 30:                            // get cursor page
      bx = CRTPage;
      break;
    default:                            // chain to the old handler
      _chain_intr(OldInt33);
      break;
  }
}

void InstallMouse(void)
{
  _AX = 0x3533;
  geninterrupt(0x21);
  OldInt33 = (void interrupt (*)())MK_FP(_ES, _BX);
  if(OldInt33 != 0 && *(unsigned char far *)OldInt33 != 0xCF)
  {
    _AX = 0;
    geninterrupt(0x33);
    if(_AX == 0xFFFF)
    {
      asm  push ds
      _DS = _CS;
      _DX = FP_OFF(Int33h);
      _AX = 0x2533;
      geninterrupt(0x21);
      asm  pop  ds
    }
  }
}

void UninstallMouse(void)
{
  asm  push ds
  _DX = FP_OFF(OldInt33);
  _DS = FP_SEG(OldInt33);
  _AX = 0x2533;
  geninterrupt(0x21);
  asm  pop  ds
  if(OldInt33)
  {
    _ES = _CX = _DX = 0;
    _AX = 12;
    geninterrupt(0x33);
  }
}
