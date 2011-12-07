TASM    = tasm
TLINK   = tlink
CC      = bcc

LINKOPT = -s -c -t -i

!if $d(BGI30)

COPT    = -mt! -1- -f- -O1 -w -DBGI30
TASMOPT = -m -ml -w -jP8086 -dBGI30

!else

COPT    = -mt! -1- -f- -O1 -w
TASMOPT = -m -ml -w -jP8086

!endif

OBJS    = segs.obj vga256.obj bitmap.obj flood.obj interfce.obj line1.obj mouse.obj patbar.obj pixel.obj text.obj

.asm.obj:
	$(TASM) $(TASMOPT) $.

.c.obj:
	$(CC) -c $(COPT) {$. }

vga256.bgi: $(OBJS)
	$(TLINK) $(LINKOPT) @&&!
$(OBJS)
$.
!

segs.obj: segs.asm
	$(TASM) $(TASMOPT) -DDriverName='VGA256' $&

vga256.obj: vga256.asm

saveds.obj: saveds.asm

bitmap.obj: bitmap.c

flood.obj: flood.c

interfce.obj: interfce.c

line1.obj: line1.c

line2.obj: line2.c

mouse.obj: mouse.c

patbar.obj: patbar.c

pixel.obj: pixel.c

text.obj: text.c

clean:
	del *.obj
	del *.bin
	del *.bak
	del *.map
