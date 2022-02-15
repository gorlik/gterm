;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2022 Gabriele Gorla                                             *
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
.module soft80
.globl _soft80_charset
	
.globl _PutLine
.globl _PutChar80
.globl _Put2Char80
.globl _ClrTo
.globl _ClrFrom

	.globl _attr
	.globl _c1, _c2
	.globl _ROW, _COL
	.globl _line_addr
	.globl _STR_BUF
	.globl _SCR_BASE
	.globl _COLOR_BASE
	.globl _SPR_DATA

	_SCR_BASE = 0x6000
	_COLOR_BASE = _SCR_BASE-0x400
        _SPR_DATA   = _COLOR_BASE-64


.area DSEG
t1:
	.db 1
t2:
	.db 1
_c1::
	.db 1
_c2::
	.db 1
_get::
	.db 1
_put::
	.db 1
_line_addr::
	.db 2
	
.area CSEG

;//CHAR_E = _soft80_charset
;//CHAR_O = _soft80_charset+0x400
	

; ****************** PutChar *********************
;  Put a character
_PutChar80:
	;; c1: input char
	;; t2: iverse mask
	;; carry bit -> invert char

;//	clc
;//	lda _attr
;//	and #0x01
;//	beq pskip
;//	sec
;//pskip:
	lda _attr
	ror a 			; put bit 1 of _attr in the carry

	lda *_c1	    ;;  a: char
	tax
	ldy #0x00
	sty *t2
	lda *_COL
	inc *_COL
	and #0x01
	bne odd
;; even char
	bcc even_skip
	lda #0xF0
	sta *t2
even_skip:
	cline = 0
.rept 7
	lda [_line_addr],y
	and #0x0F
	ora _soft80_charset+cline*128,x	;
	eor *t2
	sta [_line_addr],y
	iny
	cline = cline + 1
.endm
	lda _attr
	and #0x02
	beq skip_underline_e
;; replace line 6 with underline
	dey
	lda [_line_addr],y
	and #0x0F
	ora #0xF0
	eor *t2
	sta [_line_addr],y
	iny
skip_underline_e:	
	lda [_line_addr],y
	and #0x0F
	ora _soft80_charset+7*128,x	;
	eor *t2
	sta [_line_addr],y
	rts 			;
	
odd:
	bcc odd_skip
	lda #0x0F
	sta *t2
odd_skip:
	cline = 0
.rept 7
	lda [_line_addr],y
	and #0xF0
	ora _soft80_charset+0x400+cline*128,x	;
	eor *t2
	sta [_line_addr],y
	iny
	cline = cline + 1
.endm
	lda _attr
	and #0x02
	beq skip_underline_o
;; replace line 6 with underline
	dey
	lda [_line_addr],y
	and #0xF0
	ora #0x0F
	eor *t2
	sta [_line_addr],y
	iny
skip_underline_o:	
	lda [_line_addr],y
	and #0xF0
	ora _soft80_charset+0x400+7*128,x	;
	eor *t2
	sta [_line_addr],y
line_add:	
	lda *_line_addr
	clc
	adc #0x08
	bne no_carry_pc
	inc *_line_addr+1
no_carry_pc:
	sta *_line_addr
;//end2:	
	rts


; ****************** Put2Char *********************
;  Put two character
	;; c1: input char
	;; c2: input char
	;; t2: iverse mask
_Put2Char80:
	ldy #0x00
	lda _attr
	bne generic
	;; optimized version (no reverse and no underline)
cline = 0
.rept 8
	ldx *_c1
	lda _soft80_charset+cline*128,x
	ldx *_c2
	ora _soft80_charset+0x400+cline*128,x
	sta [_line_addr],y
	iny
	cline = cline + 1
.endm
	jmp p2c_end

;  generic version (reverse and underline support)
generic:	
	sty *t2
	lda _attr
	and #0x01 		; inverse
	beq iskip
	lda #0xff
	sta *t2
iskip:
	cline = 0
.rept 6
	ldx *_c1
	lda _soft80_charset+cline*128,x
	ldx *_c2
	ora _soft80_charset+0x400+cline*128,x
	eor *t2
	sta [_line_addr],y
	iny
	cline = cline + 1
.endm
	;; iteration 6
	lda _attr
	and #0x02
	bne underline
	ldx *_c1
	lda _soft80_charset+6*128,x
	ldx *_c2
	ora _soft80_charset+0x400+6*128,x
	eor *t2
	sta [_line_addr],y
	iny
	bne iter7
underline:
	lda #0xFF
	eor *t2
	sta [_line_addr],y
	iny
	;; iteration 7
iter7:	
	ldx *_c1
	lda _soft80_charset+7*128,x
	ldx *_c2
	ora _soft80_charset+0x400+7*128,x
	eor *t2
	sta [_line_addr],y
	;	iny

p2c_end:	
	inc *_COL
	inc *_COL
	lda *_line_addr
	clc
	adc #0x08
	bne no_carry_p2c
	inc *_line_addr+1
no_carry_p2c:
	sta *_line_addr
	rts

	
; ****************** PutLine *********************
;  Put a line of text
; FIXME: support for reverse is broken - no underline
	;;  c1, c2: temp char to print
	;; t1: counter for next char to write
	;; t2: inverse mask
_PutLine:
	ldy #0x00
start:	
	lda _STR_BUF,y
	bne cont1
	rts
cont1:	sta *_c1
	iny
	lda _STR_BUF,y
	bne cont2
	;; print the last char of the string
	jmp _PutChar80
	
cont2:
	sta *_c2
	iny
	sty *t1
	jsr _Put2Char80
	ldy *t1
	cpy #80
	bne start
;//pl_end:
 	rts

; ****************** ClrTo *********************
;  Clear to the cursor
_ClrTo:
	lda *_COL
	clc
	adc #0x01
	lsr
	tax
	jsr _Put2Blank
	lda *_COL
	clc
	adc #0x01
	and #0x01
	beq clr_end
	ldy #0x00
.rept 8
	lda [_line_addr],y
	and #0x0F
	sta [_line_addr],y
	iny
.endm
clr_end:
	rts

; ****************** ClrFrom *********************
;  Clear from the cursor
_ClrFrom:
	lda *_COL
	and #0x01
	beq skip_clr
.rept 8
	lda [_line_addr],y
	and #0xF0
	sta [_line_addr],y
	iny
.endm

	lda *_line_addr
	clc
	adc #0x08
	bne no_carry_clr
	inc *_line_addr+1
no_carry_clr:
	sta *_line_addr

skip_clr:
	clc
	lda #81 		; FIXME: should be 80 ?
	sbc *_COL
	lsr
	tax
	jmp _Put2Blank
	rts

; ****************** Put2Blank *********************
;  Put two blanks
_Put2Blank:
	;; x: number of loops
	cpx #0x00
	beq end
loop:	lda #0x00
	ldy #0x00
.rept 8
	sta [_line_addr],y
	iny
.endm
	lda *_line_addr
	clc
	adc #0x08
	bne no_carry_p2b
	inc *_line_addr+1
no_carry_p2b:
	sta *_line_addr
	dex
	bne loop
end:	
	rts
