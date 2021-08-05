;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2019 Gabriele Gorla                                             *
;*                                                                            *
;*  This program is free software: you can redistribute it and/or modify      *
;*  it under the terms of the GNU General Public License as published by      *
;*  the Free Software Foundation, either version 3 of the License, or         *
;*  (at your option) any later version.                                       *
;*                                                                            *
;*  GTERM is distributed in the hope that it will be useful,                  *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
;*  GNU General Public License for more details.                              *
;*                                                                            *
;*  You should have received a copy of the GNU General Public License         *
;*  along with GTERM.  If not, see <http://www.gnu.org/licenses/>.            *
;*                                                                            *
;******************************************************************************
.include "gterm.inc"
	
.export _ClrScr
.export _SetColor
.export _ClrLine
.export _ClrLineAddr
.export _ClrLastLine
.export _ClrFirstLine
.export _CopyLine
.export _FS_Scroll1
.export _FS_InvScroll1
.export _FS_Scroll4
.export _FS_Scroll8

.segment	"CODE"

; ****************** clrscr *********************
;  clear screen (skip title line)
.proc _ClrScr: near
	ldx #160
loop:
	lda #$00
.repeat 24,line
	sta _SCR_BASE-1+(line+1)*320,x
	sta _SCR_BASE-1+(line+1)*320+160,x	
.endrepeat
	dex
	beq done
	jmp loop
done:
	rts
	.endproc

; ****************** SetColor *********************
;  set screen color (skip title line)
.proc _SetColor: near
	ldx #240
loop:
	sta _COLOR_BASE+39,x
	sta _COLOR_BASE+39+240,x	
	sta _COLOR_BASE+39+480,x
	sta _COLOR_BASE+39+720,x
	dex
	bne loop
	rts
.endproc

; ****************** ClrLine *********************
;  clear one line
;;  _line_addr: address of line to clear
.proc _ClrLine: near
	asl
	tay
	lda _line,y
	sta _line_addr
	lda _line+1,y
	sta _line_addr+1
;;	jmp _ClrLineAddr
.endproc
	

.proc _ClrLineAddr: near
	lda #$00
	tay
loop1:
	sta (_line_addr),y
	dey
	bne loop1
	inc _line_addr+1
	ldy #63
loop2:
	sta (_line_addr),y
	dey
	bpl loop2
	rts
.endproc

; ****************** clrfirstline *********************
;  clear first line
.proc _ClrFirstLine: near
	ldx #80
	lda #$00
loop:	
	sta _SCR_BASE-1+320,x
	sta _SCR_BASE-1+320+80,x
	sta _SCR_BASE-1+320+160,x	
	sta _SCR_BASE-1+320+240,x	
	dex
	bne loop
	rts
.endproc

	
; ****************** clrlastline *********************
;  clear last line
.proc _ClrLastLine: near
	ldx #80
	lda #$00
loop:	
	sta _SCR_BASE-1+320*24,x
	sta _SCR_BASE-1+320*24+80,x
	sta _SCR_BASE-1+320*24+160,x	
	sta _SCR_BASE-1+320*24+240,x	
	dex
	bne loop
	rts
.endproc

; ****************** CopyLine *********************
;  copy line
	;; SRC_LINE: address of source line
	;; _line_addr: address of destination line
.proc _CopyLine: near
	ldy #$00
loop1:
	lda (_SRC_LINE),y
	sta (_line_addr),y
	dey
	bne loop1
	inc _line_addr+1
	inc _SRC_LINE+1
	ldy #63
loop2:
	lda (_SRC_LINE),y
	sta (_line_addr),y
	dey
	bpl loop2
	rts
.endproc

	
; ****************** Scroll 1 *********************
;  scroll the screen by one line (Skip the top line)
.proc _FS_Scroll1: near
	ldx #160
loop:	
.repeat 23,line
	lda _SCR_BASE-1+(line+2)*320,x
	sta _SCR_BASE-1+(line+1)*320,x
	lda _SCR_BASE-1+(line+2)*320+160,x
	sta _SCR_BASE-1+(line+1)*320+160,x	
.endrepeat
	dex
	beq done
	jmp loop
done:   jmp _ClrLastLine
;	rts
	.endproc

;  ****************** InvScroll ********************* 
;  inverse scroll the screen by one line (Skip the top line)
.proc _FS_InvScroll1: near
	ldx #160
loop:	
.repeat 23,line
	lda _SCR_BASE-1+(23-line)*320,x
	sta _SCR_BASE-1+(24-line)*320,x
	lda _SCR_BASE-1+(23-line)*320+160,x
	sta _SCR_BASE-1+(24-line)*320+160,x	
.endrepeat
	dex
	beq done
	jmp loop
done:   jmp _ClrFirstLine
	;	rts
.endproc


; ****************** Scroll 4 *********************
;  scroll the screen by 4 lines (Skip the top line)
.proc _FS_Scroll4: near
	ldx #160
loop:	
.repeat 20,line
	lda _SCR_BASE-1+(line+5)*320,x
	sta _SCR_BASE-1+(line+1)*320,x
	lda _SCR_BASE-1+(line+5)*320+160,x
	sta _SCR_BASE-1+(line+1)*320+160,x	
.endrepeat
	dex
	beq done
	jmp loop
done:	
	rts
.endproc

; ****************** Scroll 8 *********************
;  scroll the screen by 8 lines (Skip the top line)
.proc _FS_Scroll8: near
	ldx #160
loop:	
.repeat 16,line
	lda _SCR_BASE-1+(line+9)*320,x
	sta _SCR_BASE-1+(line+1)*320,x
	lda _SCR_BASE-1+(line+9)*320+160,x
	sta _SCR_BASE-1+(line+1)*320+160,x	
.endrepeat
	dex
	beq done
	jmp loop
done:	
	rts
.endproc
