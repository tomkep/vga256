IFDEF ??version
IF ??version ge 030Ah
   VERSION T300
ENDIF
ELSE
   DISPLAY "This module requires Turbo Assembler"
   ERR
ENDIF

IDEAL

IFDEF __TINY__
   $$Tiny       EQU
ENDIF

IFDEF __SMALL__
   $$Small      EQU
ENDIF

IFDEF __MEDIUM__
   $$Medium     EQU
ENDIF

IFDEF __COMPACT__
   $$Compact    EQU
ENDIF

IFDEF __LARGE__
   $$Large      EQU
ENDIF

IFDEF __HUGE__
   $$Huge       EQU
ENDIF

IFDEF __T__
   $$Tiny       EQU
ENDIF

IFDEF __S__
   $$Small      EQU
ENDIF

IFDEF __M__
   $$Medium     EQU
ENDIF

IFDEF __C__
   $$Compact    EQU
ENDIF

IFDEF __L__
   $$Large      EQU
ENDIF

IFDEF __H__
   $$Huge       EQU
ENDIF

IFDEF __P__
   $$Pascal     EQU
ENDIF

IFDEF __t__
   $$Tiny       EQU
ENDIF

IFDEF __s__
   $$Small      EQU
ENDIF

IFDEF __m__
   $$Medium     EQU
ENDIF

IFDEF __c__
   $$Compact    EQU
ENDIF

IFDEF __l__
   $$Large      EQU
ENDIF

IFDEF __h__
   $$Huge       EQU
ENDIF

IFDEF __p__
   $$Pascal     EQU
ENDIF

IFDEF $$Tiny
   $$Model      EQU     TINY
   $$Language   EQU     C
   $$StackPtr   EQU     NEARSTACK
ENDIF

IFDEF $$Small
   $$Model      EQU     SMALL
   $$Language   EQU     C
   $$StackPtr   EQU     NEARSTACK
ENDIF

IFDEF $$Compact
   $$Model      EQU     COMPACT
   $$Language   EQU     C
   $$StackPtr   EQU     FARSTACK
ENDIF

IFDEF $$Medium
   $$Model      EQU     MEDIUM
   $$Language   EQU     C
   $$StackPtr   EQU     NEARSTACK
ENDIF

IFDEF $$Large
   $$Model      EQU     LARGE
   $$Language   EQU     C
   $$StackPtr   EQU     FARSTACK
ENDIF

IFDEF $$Huge
IFDEF $$TurboC
   $$Model      EQU     HUGE
ELSE
   $$Model      EQU     TCHUGE
ENDIF
   $$Language   EQU     C
   $$StackPtr   EQU     FARSTACK
ENDIF

IFDEF $$Pascal
   $$Model      EQU     LARGE
   $$Language   EQU     PASCAL
   $$StackPtr   EQU     FARSTACK
ENDIF

IFNDEF $$Model
   DISPLAY "Memory model not specified."
   ERR
ENDIF

IFNDEF $$Language
IFNDEF $$StackPtr
   MODEL $$Model
ELSE
   MODEL $$StackPtr $$Model
ENDIF
ELSE
IFNDEF $$StackPtr
   MODEL $$Model, $$Language
ELSE
   MODEL $$StackPtr $$Model, $$Language
ENDIF
ENDIF

   MACRO        SaveSI
                USES    si
   ENDM
   MACRO        SaveDI
                USES    di
   ENDM

IF @DataSize eq 0
   MACRO        LES_ reg,memref
                mov     reg,memref
   ENDM
   MACRO        LDS_ reg,memref
                mov     reg,memref
   ENDM
   MACRO        SaveDS
   ENDM
   MACRO        LoadES
                push    ds
                pop     es
   ENDM
   ES_          EQU
   SS_          EQU
ELSE
   MACRO        LES_ reg,memref
                les     reg,memref
   ENDM
   MACRO        LDS_ reg,memref
                lds     reg,memref
   ENDM
   MACRO        SaveDS
                USES    ds
   ENDM
   MACRO        LoadES
   ENDM
   ES_          EQU     es:
   SS_          EQU     ss:
ENDIF

IF @Model eq 7
   MACRO        SetUpDS
                ASSUME  ds : @data
                USES    ds
                mov     ax,@data
                mov     ds,ax
   ENDM
ELSE
   MACRO        SetUpDS
   ENDM
ENDIF

   _Cdecl       EQU     C

IFDEF $$Pascal
   _CType       EQU     PASCAL
ELSEIFDEF  __PAS__
   _CType       EQU     PASCAL
ELSEIFDEF  __PASCAL__
   _CType       EQU     PASCAL
ELSE
   _CType       EQU     _Cdecl
ENDIF

IFDEF $$PM
   GLOBAL PASCAL __0000H : ABS
   GLOBAL PASCAL __0040H : ABS
   GLOBAL PASCAL __A000H : ABS
   GLOBAL PASCAL __B000H : ABS
   GLOBAL PASCAL __B800H : ABS
   GLOBAL PASCAL __C000H : ABS
   GLOBAL PASCAL __D000H : ABS
   GLOBAL PASCAL __E000H : ABS
   GLOBAL PASCAL __F000H : ABS
   Sel0000h     EQU     __0000H
   Sel0040h     EQU     __0040H
   SelA000h     EQU     __A000H
   SelB000h     EQU     __B000H
   SelB800h     EQU     __B800H
   SelC000h     EQU     __C000H
   SelD000h     EQU     __D000H
   SelE000h     EQU     __E000H
   SelF000h     EQU     __F000H
ELSE
   Sel0000h     =       00000h
   Sel0040h     =       00040h
   SelA000h     =       0A000h
   SelB000h     =       0B000h
   SelB800h     =       0B800h
   SelC000h     =       0C000h
   SelD000h     =       0D000h
   SelE000h     =       0E000h
   SelF000h     =       0F000h
ENDIF

IF @DataSize eq 0
        $$ES    EQU
        $$DS    EQU
        $$SS    EQU
ELSE
        $$ES    EQU     es
        $$DS    EQU     ds
        $$SS    EQU     ss
ENDIF

IF @CodeSize eq 0
        $$CS    EQU
ELSE
        $$CS    EQU     cs
ENDIF

MACRO CODESTART
IF @CodeSize EQ 0
CODESEG
ELSEIFNDEF $$CodeSeg
CODESEG
ELSEIFDEF $$Pascal
CODESEG
ELSE
CODESEG $$CodeSeg
ENDIF
ENDM

IF @CodeSize EQ 0
        $$CodeSize      =       2
ELSE
        $$CodeSize      =       4
ENDIF

IF @DataSize EQ 0
        $$DataSize      =       2
ELSE
        $$DataSize      =       4
ENDIF
