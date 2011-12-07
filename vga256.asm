IDEAL

IFDEF BGI30

$$Small         EQU

ELSE

$$Tiny          EQU

ENDIF

INCLUDE "macros.inc"
INCLUDE "device.inc"

PUBLIC PASCAL   DGROUP@
PUBLIC PASCAL   N_SCOPY@
PUBLIC _CType   _chain_intr
PUBLIC _CType   VideoSeg
PUBLIC _CType   SelA000
PUBLIC _CType   Sel0040

IFDEF BGI30

PUBLIC _CType   CpuMode
PUBLIC _CType   FillMem

ELSE

PUBLIC _CType   SaveDataSeg

ENDIF

EXTRN   _CType  pINSTALL        :       PROC
EXTRN   _CType  pINIT           :       PROC
EXTRN   _CType  pCLEAR          :       PROC
EXTRN   _CType  pPOST           :       PROC
EXTRN   _CType  pMOVE           :       PROC
EXTRN   _CType  pDRAW           :       PROC
EXTRN   _CType  pVECT           :       PROC
EXTRN   _CType  pPATBAR         :       PROC
EXTRN   _CType  pPALETTE        :       PROC
EXTRN   _CType  pALLPALETTE     :       PROC
EXTRN   _CType  pCOLOR          :       PROC
EXTRN   _CType  pFILLSTYLE      :       PROC
EXTRN   _CType  pLINESTYLE      :       PROC
EXTRN   _CType  pTEXTSTYLE      :       PROC
EXTRN   _CType  pTEXT           :       PROC
EXTRN   _CType  pTEXTSIZ        :       PROC
EXTRN   _CType  pFLOODFILL      :       PROC
EXTRN   _CType  pGETPIX         :       PROC
EXTRN   _CType  pPUTPIX         :       PROC
EXTRN   _CType  pBITMAPUTIL     :       PROC
EXTRN   _CType  pSAVEBITMAP     :       PROC
EXTRN   _CType  pRESTOREBITMAP  :       PROC
EXTRN   _CType  pSETCLIP        :       PROC
EXTRN   _CType  pCOLOR_QUERY    :       PROC

CODESEG
                BGI     VGA256

DATASEG

IFDEF BGI30

                db      2               ; number of selectors to be converted
                dw      offset SelA000  ; offset to selector table
                db      0               ; number of interrupt addresses to be converted
                dw      0               ; offset to table of doublewords with interrupt number in lowest byte
                db      0               ; number of buffers to allocate
                dw      0               ; address of triplewords table
                                        ; the first word  -> number of paragraphs to allocate
                                        ; the second word -> gets real mode buffer address
                                        ; the third word  -> gets protected mode buffer address
                ORG     0Ah
                dw      0               ; data segment selector
CpuMode         db      0               ; 0 -> real mode, 1 -> protected mode

ENDIF

LABEL           VideoSeg        WORD
SelA000         dw      0A000h
Sel0040         dw      00040h

DDOVEC          dw      pINSTALL
                dw      pINIT
                dw      pCLEAR
                dw      pPOST
                dw      pMOVE
                dw      pDRAW
                dw      pVECT
                dw      EMULATE
                dw      EMULATE
                dw      pPATBAR
                dw      EMULATE
                dw      EMULATE
                dw      EMULATE
                dw      pPALETTE
                dw      pALLPALETTE
                dw      pCOLOR
                dw      pFILLSTYLE
                dw      pLINESTYLE
                dw      pTEXTSTYLE
                dw      pTEXT
                dw      pTEXTSIZ
                dw      RESERVED
                dw      pFLOODFILL
                dw      pGETPIX
                dw      pPUTPIX
                dw      pBITMAPUTIL
                dw      pSAVEBITMAP
                dw      pRESTOREBITMAP
                dw      pSETCLIP
                dw      pCOLOR_QUERY
                dw      35 DUP (RESERVED)

CODESEG

PROC _CType     _chain_intr
IF @CodeSize
                add     sp,4
ELSE
                add     sp,2
ENDIF
                mov     bx,[bp+14]
                mov     ax,[bp+16]
                pop     [word bp+14]
                pop     [word bp+16]
                mov     sp,bp
                pop     bp
                pop     di
                pop     si
                pop     ds
                pop     es
                pop     dx
                pop     cx
                cli
                retf
ENDP

PROC PASCAL     N_SCOPY@        NEAR
                ARG     @@dst : DWORD, @@src : DWORD
                USES    ds, si, di
                cld
                les     di,[@@dst]
                lds     si,[@@src]
                shr     cx,1
                jnc     @@1
                movsb
        @@1:    rep     movsw
                ret
ENDP

IFDEF           BGI30

PROC _CType     FillMem
                ARG     @@ptr : DWORD, @@c : BYTE, @@n : WORD
                USES    di
                les     di,[@@ptr]
                mov     cx,[@@n]
                mov     al,[@@c]
                mov     ah,al
                shr     cx,1
                jnc     @@1
                stosb
        @@1:    rep     stosw
                ret
ENDP

ELSE

PROC _CType     SaveDataSeg
                mov     [DGROUP@],ds
                ret

ENDP
ENDIF

END
