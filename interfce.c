#include "vga256.h"

PALETTE DefaultPalette =
{                /* Define the palette for above EGA mode */
  16,
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
  }
};

STATUS status[2] =
{
  {
    0,
    0,
    319,
    199,
    319,
    199,
    10000,
    7020,
    8900,
    8,
    8,
    144,
    144
  },
  {
    0,
    0,
    319,
    399,
    319,
    399,
    10000,
    7020,
    17800,
    8,
    8,
    144,
    144
  }
};

char ModeName[2][26] =
{
  "\x18VGA 320 x 200 256 colors",
  "\x18VGA 320 x 400 256 colors"
};

unsigned char near *VideoOff = 0;

int ModeNo;

RECT vp;
POINT cp =
{
  0,
  0
};

int DrawColor;
int BkgrColor;
PutPixelFunc_t PutPixelFunc = PutPixel;

#ifdef BGI30

struct DPMICallStruct
{
  unsigned long EDI;
  unsigned long ESI;
  unsigned long EBP;
  unsigned long reserved;
  unsigned long EBX;
  unsigned long EDX;
  unsigned long ECX;
  unsigned long EAX;
  unsigned FLAGS;
  unsigned ES;
  unsigned DS;
  unsigned FS;
  unsigned GS;
  unsigned IP;
  unsigned CS;
  unsigned SP;
  unsigned SS;
};

extern unsigned char CpuMode;

void FillMem(void far *ptr, int c, unsigned n);

int CallRealInt(int IntNo, struct REGPACK far *regs)
{
  struct DPMICallStruct CS;
  FillMem(&CS, 0, sizeof(CS));
  CS.EAX = regs->r_ax;
  CS.EBX = regs->r_bx;
  CS.ECX = regs->r_cx;
  CS.EDX = regs->r_dx;
  CS.ESI = regs->r_si;
  CS.EDI = regs->r_di;
  CS.EBP = regs->r_bp;
  CS.FLAGS = regs->r_flags;
  CS.DS = regs->r_ds;
  CS.ES = regs->r_es;
  _ES = FP_SEG(&CS);
  _DI = FP_OFF(&CS);
  _BL = IntNo;
  _BH = 0;
  _CX = 0;
  _AX = 0x300;
  geninterrupt(0x31);
  asm   jc      label_ret
  regs->r_ax = CS.EAX;
  regs->r_bx = CS.EBX;
  regs->r_cx = CS.ECX;
  regs->r_dx = CS.EDX;
  regs->r_si = CS.ESI;
  regs->r_di = CS.EDI;
  regs->r_bp = CS.EBP;
  regs->r_flags = CS.FLAGS;
  regs->r_ds = regs->r_es = 0;
  if(CS.DS != 0)
  {
    _BX = CS.DS;
    _AX = 2;
    geninterrupt(0x31);
    asm jc      label_ret
    regs->r_ds = _AX;
  }
  if(CS.ES != 0)
  {
    _BX = CS.ES;
    _AX = 2;
    geninterrupt(0x31);
    asm jc      label_ret
    regs->r_es = _AX;
  }
  label_ret:
  asm   sbb     ax,ax
  return (_AX);
}

#pragma saveregs
void pINSTALL(void)
{
  unsigned char al = _AL, cl = _CL;
  struct REGPACK regs;
  switch(al)
  {
    case 0:
      ModeNo = cl;
      vp.x1 = vp.y1 = 0;
      vp.x2 = status[ModeNo].xres;
      vp.y2 = status[ModeNo].yres;
      if(CpuMode == 0)
      {
        asm   push    bp
        _AX = 0x1130;
        _BH = 3;
        geninterrupt(0x10);
        Font8x8Lo = (unsigned char far *)MK_FP(_ES, _BP);
        _AX = 0x1130;
        _BH = 4;
        geninterrupt(0x10);
        Font8x8Hi = (unsigned char far *)MK_FP(_ES, _BP);
        asm   pop     bp
      }
      else
      {
        FillMem(&regs, 0, sizeof(regs));
        regs.r_ax = 0x1130;
        regs.r_bx = 0x0300;
        if(CallRealInt(0x10, &regs) == 0)
          Font8x8Lo = (unsigned char far *)MK_FP(regs.r_es, regs.r_bp);
        else
          status[ModeNo].stat = grError;
        FillMem(&regs, 0, sizeof(regs));
        regs.r_ax = 0x1130;
        regs.r_bx = 0x0400;
        if(CallRealInt(0x10, &regs) == 0)
          Font8x8Hi = (unsigned char far *)MK_FP(regs.r_es, regs.r_bp);
        else
          status[ModeNo].stat = grError;
      }
      InstallMouse();
      _BX = FP_OFF(&status[ModeNo]);
      _ES = FP_SEG(status);
      break;
    case 1:
      _CX = 2;
      break;
    case 2:
      _BX = FP_OFF(&ModeName[_CX]);
      _ES = FP_SEG(ModeName);
      break;
  }
}

#else

#pragma saveregs
void pINSTALL(void)
{
  void SaveDataSeg(void);
  unsigned char al = _AL, cl = _CL;
  switch(al)
  {
    case 0:
      SaveDataSeg();
      ModeNo = cl;
      vp.x1 = vp.y1 = 0;
      vp.x2 = status[ModeNo].xres;
      vp.y2 = status[ModeNo].yres;
      asm   push    bp
      _AX = 0x1130;
      _BH = 3;
      geninterrupt(0x10);
      Font8x8Lo = (unsigned char far *)MK_FP(_ES, _BP);
      _AX = 0x1130;
      _BH = 4;
      geninterrupt(0x10);
      Font8x8Hi = (unsigned char far *)MK_FP(_ES, _BP);
      asm   pop     bp
      InstallMouse();
      _BX = FP_OFF(&status[ModeNo]);
      _ES = FP_SEG(status);
      break;
    case 1:
      _CX = 2;
      break;
    case 2:
      _BX = FP_OFF(&ModeName[_CX]);
      _ES = FP_SEG(ModeName);
      break;
  }
}

#endif

#pragma saveregs
void pCLEAR(void)
{
  outportw(0x3C4, 0x0F02);
  outportw(0x3CE, 3);
  _CX = ModeNo ? 16000 : 8000;
  _ES = (unsigned)VideoSeg;
  _DI = (unsigned)VideoOff;
  _AX = 0;
  asm   rep     stosw
}

#pragma saveregs
void pPOST(void)
{
  UninstallMouse();
}

#pragma saveregs
void pMOVE(void)
{
  cp.x = _AX;
  cp.y = _BX;
}

#pragma saveregs
void pDRAW(void)
{
  unsigned ax = _AX, bx = _BX;
  Vect(cp.x, cp.y, ax, bx);
  cp.x = ax;
  cp.y = bx;
}

#pragma saveregs
void pVECT(void)
{
  Vect(_AX, _BX, _CX, _DX);
}

#pragma saveregs
void pPATBAR(void)
{
  PatBar(_AX, _BX, _CX, _DX);
}

#pragma saveregs
void pPALETTE(void)
{
  asm   test    ah,0x80
  asm   jz      lab_1
  asm   test    ah,0x40
  asm   jz      lab_2
  _AL = 0;
  lab_1:
  _BH = _BL;
  _BL = _AL;
  _AX = 0x1000;
  geninterrupt(0x10);
  return;
  lab_2:
  _DH = _BL;
  _CH = _CL;
  _CL = _DL;
  _BH = 0;
  _BL = _AL;
  _AX = 0x1010;
  geninterrupt(0x10);
}

#pragma saveregs
void pALLPALETTE(void)
{
  _AX = 0x1002;
  _DX = _BX;
  geninterrupt(0x10);
}

#pragma saveregs
void pCOLOR(void)
{
  unsigned char al = _AL, ah = _AH;
  DrawColor = al;
  FillColor = ah;
}

#pragma saveregs
void pFILLSTYLE(void)
{
  SetFillStyle(_AL, (unsigned char far *)MK_FP(_ES, _BX));
}

#pragma saveregs
void pLINESTYLE(void)
{
  SetLineStyle(_AL, _BX, _CX);
}

#pragma saveregs
void pTEXTSTYLE(void)
{
  unsigned char al = _AL, ah = _AH;
  SetTextStyle(al, ah, _BX, _CX);
  _BX = _AX;
  _CX = _DX;
}

#pragma saveregs
void pTEXT(void)
{
  Text((char far *)MK_FP(_ES, _BX), _CX);
}

#pragma saveregs
void pTEXTSIZ(void)
{
  GetTextSize((char far *)MK_FP(_ES, _BX), _CX);
  _BX = _AX;
  _CX = _DX;
}

#pragma saveregs
void pFLOODFILL(void)
{
  FloodFill(_AX, _BX, _CL);
}

#pragma saveregs
void pGETPIX(void)
{
  _DL = GetPixel(_AX, _BX);
}

#pragma saveregs
void pPUTPIX(void)
{
  PutPixel(_AX, _BX, _DX);
}

static void far fNULL(void)
{
}

#pragma saveregs
static void huge fGETPIXEL(void)
{
  pGETPIX();
}

#pragma saveregs
static void huge fPUTPIXEL(void)
{
  pPUTPIX();
}

static void far fGETPIXBYTE(void)
{
  _AX = 8;
}

#pragma saveregs
static void huge fSET_DRAW_PAGE(void)
{
  VideoOff = (unsigned char near *)(ModeNo ? (_AX & 1) << 15 : (_AX & 3) << 14);
}

#pragma saveregs
static void huge fSET_VISUAL_PAGE(void)
{
  if(ModeNo)
    outportw(0x3D4, (_AL & 1) << 15 | 0x0C);
  else
    outportw(0x3D4, (_AL & 3) << 14 | 0x0C);
}

#pragma saveregs
static void huge fSET_WRITE_MODE(void)
{
  PutPixelFunc = _AX ? XorPutPixel : PutPixel;
}

static void (* near BitmapUtils[])(void) =
{
  (void (* near)(void))fNULL,
  (void (* near)(void))fNULL,
  (void (* near)(void))fPUTPIXEL,
  (void (* near)(void))fGETPIXEL,
  (void (* near)(void))fGETPIXBYTE,
  (void (* near)(void))fSET_DRAW_PAGE,
  (void (* near)(void))fSET_VISUAL_PAGE,
  (void (* near)(void))fSET_WRITE_MODE
};

#pragma saveregs
void pBITMAPUTIL(void)
{
  _BX = FP_OFF(BitmapUtils);
  _ES = FP_SEG(BitmapUtils);
}

#pragma saveregs
void pSAVEBITMAP(void)
{
  SaveBitmap(_CX, _DX, MK_FP(_ES, _BX));
}

#pragma saveregs
void pRESTOREBITMAP(void)
{
  RestoreBitmap(_CX, _DX, _AL, MK_FP(_ES, _BX));
}

#pragma saveregs
void pSETCLIP(void)
{
  vp.x1 = _AX;
  vp.y1 = _BX;
  vp.x2 = _CX;
  vp.y2 = _DX;
}

#pragma saveregs
void pCOLOR_QUERY(void)
{
  if(!_AL)
  {
    _CX = 255;
    _BX = 16;
  }
  else
  {
    _BX = FP_OFF(&DefaultPalette);
    _ES = _DS;
  }
}

#pragma saveregs
void pINIT(void)
{
  if(*((unsigned char far *)MK_FP(_ES, _BX) + 1) != 0xA5)
  {
    _AX = 0x13;
    geninterrupt(0x10);
    BkgrColor = _BH = *(unsigned char far *)MK_FP(_ES, _BX);
    _BL = 0;
    _AX = 0x1000;
    geninterrupt(0x10);
    outportb(0x3CE, 5);
    outportb(0x3CF, inportb(0x3CF) & 0xEF);
    outportb(0x3CE, 6);
    outportb(0x3CF, inportb(0x3CF) & 0xFD);
    outportb(0x3C4, 4);
    outportb(0x3C5, inportb(0x3C5) & 0xF7 | 4);
    _ES = (unsigned)VideoSeg;
    _CX = 0x8000;
    _DI = _AX = 0;
    asm rep     stosw
    if(ModeNo == 1)
    {
      outportb(0x3D4, 9);
      outportb(0x3D5, inportb(0x3D5) & 0x70);
    }
    outportb(0x3D4, 0x14);
    outportb(0x3D5, inportb(0x3D5) & 0xBF);
    outportb(0x3D4, 0x17);
    outportb(0x3D5, inportb(0x3D5) | 0x40);
    _AX = 0;
    geninterrupt(0x33);
  }
}
