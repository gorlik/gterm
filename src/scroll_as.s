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
				;//.include "gterm.inc"
.module scroll80
	
.globl _ClrScr
.globl _SetColor
.globl _ClrLine
.globl _ClrLineAddr
.globl _ClrLastLine
.globl _ClrFirstLine
.globl _CopyLine
.globl _FS_Scroll1
.globl _FS_InvScroll1
;//.globl _FS_Scroll4
;//.globl _FS_Scroll8

.globl _SCR_BASE
.globl _line_addr
.globl _COLOR_BASE
.globl _line
.globl _SRC_LINE
	

.area	CSEG

; ****************** clrscr *********************
;  clear screen (skip title line)
_ClrScr:
	ldx #160
clr_loop:
	lda #0x00
	line = 0
.rept 24
	sta _SCR_BASE-1+(line+1)*320,x
	sta _SCR_BASE-1+(line+1)*320+160,x
	line = line + 1
.endm
	dex
	beq clr_done
	jmp clr_loop
clr_done:
	rts


; ****************** SetColor *********************
;  set screen color (skip title line)
_SetColor: 
	ldx #240
color_loop:
	sta _COLOR_BASE+39,x
	sta _COLOR_BASE+39+240,x	
	sta _COLOR_BASE+39+480,x
	sta _COLOR_BASE+39+720,x
	dex
	bne color_loop
	rts


; ****************** ClrLine *********************
;  clear one line
;;  _line_addr: address of line to clear
_ClrLine:
	asl
	tay
	lda _line,y
	sta *_line_addr
	lda _line+1,y
	sta *_line_addr+1
;;	jmp _ClrLineAddr
	

_ClrLineAddr:
	lda #0x00
	tay
cl_loop1:
	sta [_line_addr],y
	dey
	bne cl_loop1
	inc *_line_addr+1
	ldy #63
cl_loop2:
	sta [_line_addr],y
	dey
	bpl cl_loop2
	rts

; ****************** clrfirstline *********************
;  clear first line
_ClrFirstLine:
	ldx #80
	lda #0x00
clrf_loop:	
	sta _SCR_BASE-1+320,x
	sta _SCR_BASE-1+320+80,x
	sta _SCR_BASE-1+320+160,x	
	sta _SCR_BASE-1+320+240,x	
	dex
	bne clrf_loop
	rts

	
; ****************** clrlastline *********************
;  clear last line
_ClrLastLine:
	ldx #80
	lda #0x00
clrl_loop:	
	sta _SCR_BASE-1+320*24,x
	sta _SCR_BASE-1+320*24+80,x
	sta _SCR_BASE-1+320*24+160,x	
	sta _SCR_BASE-1+320*24+240,x	
	dex
	bne clrl_loop
	rts

; ****************** CopyLine *********************
;  copy line
	;; SRC_LINE: address of source line
	;; _line_addr: address of destination line
_CopyLine:
	ldy #0x00
cpyl_loop1:
	lda [_SRC_LINE],y
	sta [_line_addr],y
	dey
	bne cpyl_loop1
	inc *_line_addr+1
	inc *_SRC_LINE+1
	ldy #63
cpyl_loop2:
	lda [_SRC_LINE],y
	sta [_line_addr],y
	dey
	bpl cpyl_loop2
	rts

	
; ****************** Scroll 1 *********************
;  scroll the screen by one line (Skip the top line)
_FS_Scroll1:
	ldx #160
fs_loop:	
line = 0
.rept 23
	lda _SCR_BASE-1+(line+2)*320,x
	sta _SCR_BASE-1+(line+1)*320,x
	lda _SCR_BASE-1+(line+2)*320+160,x
	sta _SCR_BASE-1+(line+1)*320+160,x
	line = line + 1
.endm
	dex
	beq fs_done
	jmp fs_loop
fs_done:   jmp _ClrLastLine
;	rts


;  ****************** InvScroll ********************* 
;  inverse scroll the screen by one line (Skip the top line)
_FS_InvScroll1:
	ldx #160
fsi_loop:
	line = 0
.rept 23
	lda _SCR_BASE-1+(23-line)*320,x
	sta _SCR_BASE-1+(24-line)*320,x
	lda _SCR_BASE-1+(23-line)*320+160,x
	sta _SCR_BASE-1+(24-line)*320+160,x
	line = line + 1
.endm
	dex
	beq fsi_done
	jmp fsi_loop
fsi_done:   jmp _ClrFirstLine
	;	rts


.if 0
; ****************** Scroll 4 *********************
;  scroll the screen by 4 lines (Skip the top line)
_FS_Scroll4:
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
.endif
