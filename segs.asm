IDEAL

IFDEF DriverName

IFDEF BGI30

SEGMENT         HEADER  PARA PRIVATE 'HEADER'
                db      'FBGD', 8, 8, 8, 8
                db      'BGI Device Driver (', DriverName, ') 3.00 - ', ??date, 13, 10
                db      'Copyright (c) Tomasz Kepczynski', 13, 10
                db      0, 26
HeaderSize1     dw      HeaderEnd@
DriverNum1      dw      0
CodeSize1       dw      CodeEnd@
DriverVer1      db      3
DriverRev1      db      0
MinVer1         db      3
MinRev1         db      0
CodeSize2       dw      CodeEnd@
DataSize2       dw      DataEnd@
                ORG     128
HeaderSize2     dw      HeaderEnd@
DriverNum2      dw      0
CodeSize3       dw      CodeEnd@
DriverVer2      db      3
DriverRev2      db      0
MinVer2         db      3
MinRev2         db      0
CodeSize4       dw      CodeEnd@
DataSize4       dw      DataEnd@
                db      SIZE DeviceName
DeviceName      db      DriverName
                ORG     160
LABEL           HeaderEnd@      UNKNOWN
ENDS

SEGMENT         _TEXT   BYTE PUBLIC 'CODE'
ENDS

SEGMENT         E_TEXT  PARA PUBLIC 'ECODE'
LABEL           CodeEnd@        UNKNOWN
ENDS

SEGMENT         _DATA   PARA PUBLIC 'DATA'
ENDS

SEGMENT         _BSS    BYTE PUBLIC 'BSS'
ENDS

SEGMENT         E_DATA  BYTE PUBLIC 'EDATA'
LABEL           DataEnd@        UNKNOWN
ENDS

GROUP           CGROUP  _TEXT, E_TEXT
GROUP           DGROUP _DATA, _BSS, E_DATA

ELSE

SEGMENT         HEADER  PARA PRIVATE 'HEADER'
                db      'pk', 8, 8
                db      'BGI Device Driver (', DriverName, ') 2.00 - ', ??date, 13, 10
                db      'Copyright (c) Tomasz Kepczynski', 13, 10
                db      0, 26
HeaderSize1     dw      HeaderEnd@
DriverNum1      dw      0
DriverSize1     dw      DriverEnd@
DriverVer1      db      2
DriverRev1      db      0
MinVer1         db      1
MinRev1         db      1
                ORG     128
HeaderSize2     dw      HeaderEnd@
DriverNum2      dw      0
DriverSize2     dw      DriverEnd@
DriverVer2      db      2
DriverRev2      db      0
MinVer2         db      1
MinRev2         db      1
                db      SIZE DeviceName
DeviceName      db      DriverName
                ORG     160
LABEL           HeaderEnd@      UNKNOWN
ENDS

SEGMENT         _TEXT   BYTE PUBLIC 'CODE'
ENDS

SEGMENT         _DATA   PARA PUBLIC 'DATA'
ENDS

SEGMENT         _BSS    BYTE PUBLIC 'BSS'
ENDS

SEGMENT         E_DATA  BYTE PUBLIC 'EDATA'
LABEL           DriverEnd@      UNKNOWN
ENDS

GROUP           DGROUP  _TEXT, _DATA, _BSS, E_DATA

ENDIF

ELSE

DISPLAY "DriverName must be defined"
ERR

ENDIF

END
