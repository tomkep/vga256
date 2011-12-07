#ifndef __VGA256_H
#define __VGA256_H

#include <dos.h>

enum put_codes
{
  CopyPut =  0,
  XorPut  =  1,
  OrPut   =  2,
  AndPut  =  3,
  NotPut  =  4
};

enum graphics_errors
{
  grOk                  =   0,
  grNoInitGraph         =  -1,
  grNotDetected         =  -2,
  grFileNotFound        =  -3,
  grInvalidDriver       =  -4,
  grNoLoadMem           =  -5,
  grNoScanMem           =  -6,
  grNoFloodMem          =  -7,
  grFontNotFound        =  -8,
  grNoFontMem           =  -9,
  grInvalidMode         = -10,
  grError               = -11,   /* generic error */
  grIOerror             = -12,
  grInvalidFont         = -13,
  grInvalidFontNum      = -14,
  grInvalidDeviceNum    = -15,
  grInvalidVersion      = -18
};

enum font_orients
{
  fntNormalOrient       = 0,
  fntVerticalOrient     = 1,
  fntDownOrient         = 2,
  fntVerticalDownOrient = 3
};

typedef struct {
  unsigned char length;   /* # of color entries in palette*/
  unsigned char color[16];/* Up to 16 color entries       */
} PALETTE;

typedef struct
{
  int   x1;            /* Upper left corner x coordinate  */
  int   y1;            /* Upper left corner y coordinate  */
  int   x2;            /* Lower right corner x coordinate */
  int   y2;            /* Lower right corner y coordinate */
} RECT;

typedef struct {
  int x;
  int y;
} POINT;

typedef struct {
  unsigned char stat;   /* Current device status.         */
  unsigned char devtype;/* Device Type Identifier.        */
  unsigned int  xres;   /* Device Full Resolution in X    */
  unsigned int  yres;   /* Device Full Resolution in Y    */
  unsigned int  xefres; /* Device Effective X Resolution  */
  unsigned int  yefres; /* Device Effective Y Resolution  */
  unsigned int  xinch;  /* Device X Size in inches * 1000 */
  unsigned int  yinch;  /* Device Y Size in inches * 1000 */
  unsigned int  aspec;  /* Aspect Ratio * 10000           */
  unsigned char chsizx; /* Standard char size X           */
  unsigned char chsizy; /* Standard char size Y           */
  unsigned char fcolors;/* Number of foreground colors    */
  unsigned char bcolors;/* Number of background colors    */
} STATUS;

extern STATUS status[2];
extern char ModeName[2][26];

extern void _seg * const VideoSeg;
extern unsigned char near *VideoOff;

extern void _seg * const SelA000;
extern void _seg * const Sel0040;

extern int ModeNo;
extern unsigned char far *Font8x8Lo;
extern unsigned char far *Font8x8Hi;

extern RECT vp;
extern POINT cp;

extern int DrawColor;
extern int BkgrColor;
extern int FillColor;

typedef void (*PutPixelFunc_t)(int, int, int);
extern PutPixelFunc_t PutPixelFunc;

extern PALETTE DefaultPalette;

#define _crotr(val, count) \
  (_CL = (count), _AL = (val), __emit__(0xD2, 0xC8), _AL)

#define _crotl(val, count) \
  (_CL = (count), _AL = (val), __emit__(0xD2, 0xC0), _AL)

#if !defined(__BORLANDC__) || __BORLANDC__ < 0x0310

#define _rotr(val, count) \
  (_CL = (count), _AX = (val), __emit__(0xD3, 0xC8), _AX)

#define _rotl(val, count) \
  (_CL = (count), _AX = (val), __emit__(0xD3, 0xC0), _AX)

#endif


#if defined(__BORLANDC__) && __BORLANDC__ >= 0x0310

#define outportw outport

#endif

int GetPixel(int x, int y);
void PutPixel(int x, int y, int color);
void XorPutPixel(int x, int y, int color);
void OrPutPixel(int x, int y, int color);
void AndPutPixel(int x, int y, int color);
void NotPutPixel(int x, int y, int color);
void Vect(int x1, int y1, int x2, int y2);
void SetLineStyle(unsigned char Pat,
                  int UserPat, int LineWidth);
void SetFillStyle(char Pat, unsigned char far *UserPat);
void PatBar(int x1, int y1, int x2, int y2);
void FloodFill(int x, int y, unsigned char color);
POINT SetTextStyle(unsigned char NewFontNo,
                   unsigned char NewFontOrient,
                   int XSize, int YSize);
POINT GetTextSize(char far *string, int len);
void Text(char far *string, int len);

typedef struct
{
  unsigned int dx;
  unsigned int dy;
  unsigned char bitmap[];
} Bitmap_t;

void SaveBitmap(int x1, int y1, Bitmap_t far *ptr);
void RestoreBitmap(int x1, int y1, char mode, Bitmap_t far *ptr);

void InstallMouse(void);
void UninstallMouse(void);

void _chain_intr(void interrupt (*)());

#endif // __VGA256_H
