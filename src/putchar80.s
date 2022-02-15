;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2019-2022 Gabriele Gorla                                        *
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

CHAR_E=_soft80_charset
CHAR_O=(_soft80_charset+$400)
	
.export _PutLine
.export _PutChar80
.export _Put2Char80
.export _ClrTo
.export _ClrFrom	

.segment "ZP_2" : zeropage
t1:
	.res 1
t2:
	.res 1
	
.segment	"CODE"

; ****************** PutChar *********************
;  Put a character
.proc _PutChar80: near
	;; c1: input char
	;; t2: iverse mask
	;; carry bit -> invert char

	lda _attr
	ror a 			; put bit 1 of _attr in the carry

	lda _c1	    ;;  a: char
	tax
	ldy #$00
	sty t2
	lda _COL
	inc _COL
	and #$01
	bne odd
;; even char
	bcc even_skip
	lda #$F0
	sta t2
even_skip:	
.repeat 7,cline
	lda (_line_addr),y
	and #$0F
	ora CHAR_E+cline*128,x	;
	eor t2
	sta (_line_addr),y
	iny
.endrepeat
	lda _attr
	and #$02
	beq skip_underline_e
;; replace line 6 with underline
	dey
	lda (_line_addr),y
	and #$0F
	ora #$F0
	eor t2
	sta (_line_addr),y
	iny
skip_underline_e:	
	lda (_line_addr),y
	and #$0F
	ora CHAR_E+7*128,x	;
	eor t2
	sta (_line_addr),y
	rts 			;
	
odd:
	bcc odd_skip
	lda #$0F
	sta t2
odd_skip:	
.repeat 7,cline
	lda (_line_addr),y
	and #$F0
	ora CHAR_O+cline*128,x	;
	eor t2
	sta (_line_addr),y
	iny
.endrepeat
	lda _attr
	and #$02
	beq skip_underline_o
;; replace line 6 with underline
	dey
	lda (_line_addr),y
	and #$F0
	ora #$0F
	eor t2
	sta (_line_addr),y
	iny
skip_underline_o:	
	lda (_line_addr),y
	and #$F0
	ora CHAR_O+7*128,x	;
	eor t2
	sta (_line_addr),y
line_add:	
	lda _line_addr
	clc
	adc #$08
	bne no_carry
	inc _line_addr+1
no_carry:
	sta _line_addr
end2:	
	rts
.endproc



; ****************** Put2Char *********************
;  Put two character
	;; c1: input char
	;; c2: input char
	;; t2: iverse mask
.proc _Put2Char80: near
	ldy #$00
	lda _attr
	bne generic
	;; optimized version (no reverse and no underline)
.repeat 8,cline
	ldx _c1
	lda CHAR_E+cline*128,x
	ldx _c2
	ora CHAR_O+cline*128,x
	sta (_line_addr),y
	iny
.endrepeat
	jmp end

;  generic version (reverse and underline support)
generic:	
	sty t2
	lda _attr
	and #$01 		; inverse
	beq iskip
	lda #$ff
	sta t2
iskip:	
.repeat 6,cline
	ldx _c1
	lda CHAR_E+cline*128,x
	ldx _c2
	ora CHAR_O+cline*128,x
	eor t2
	sta (_line_addr),y
	iny
.endrepeat
	;; iteration 6
	lda _attr
	and #$02
	bne underline
	ldx _c1
	lda CHAR_E+6*128,x
	ldx _c2
	ora CHAR_O+6*128,x
	eor t2
	sta (_line_addr),y
	iny
	bne iter7
underline:
	lda #$FF
	eor t2
	sta (_line_addr),y
	iny
	;; iteration 7
iter7:	
	ldx _c1
	lda CHAR_E+7*128,x
	ldx _c2
	ora CHAR_O+7*128,x
	eor t2
	sta (_line_addr),y
	;	iny

end:	
	inc _COL
	inc _COL
	lda _line_addr
	clc
	adc #$08
	bne no_carry2
	inc _line_addr+1
no_carry2:
	sta _line_addr
	rts
.endproc
	
; ****************** PutLine *********************
;  Put a line of text
; FIXME: support for reverse is broken - no underline
	;;  c1, c2: temp char to print
	;; t1: counter for next char to write
	;; t2: inverse mask
.proc _PutLine: near
	ldy #$00
start:	
	lda _STR_BUF,y
	bne cont1
	rts
cont1:	sta _c1
	iny
	lda _STR_BUF,y
	bne cont2
	;; print the last char of the string
	jmp _PutChar80
	
cont2:
	sta _c2
	iny
	sty t1
	jsr _Put2Char80
	ldy t1
	cpy #80
	bne start
end:
 	rts
.endproc

; ****************** ClrTo *********************
;  Clear to the cursor
.proc _ClrTo: near
	lda _COL
	clc
	adc #$01
	lsr
	tax
	jsr _Put2Blank
	lda _COL
	clc
	adc #$01
	and #$01
	beq end
	ldy #$00
.repeat 8,cline
	lda (_line_addr),y
	and #$0F
	sta (_line_addr),y
	iny
.endrepeat
end:
	rts
.endproc


; ****************** ClrFrom *********************
;  Clear from the cursor
.proc _ClrFrom: near
	lda _COL
	and #$01
	beq skip
.repeat 8,cline
	lda (_line_addr),y
	and #$F0
	sta (_line_addr),y
	iny
.endrepeat

	lda _line_addr
	clc
	adc #$08
	bne no_carry
	inc _line_addr+1
no_carry:
	sta _line_addr

skip:
	clc
	lda #81 		; FIXME: should be 80 ?
	sbc _COL
	lsr
	tax
	jmp _Put2Blank
	rts
.endproc

; ****************** Put2Blank *********************
;  Put two blanks
.proc _Put2Blank: near
	;; x: number of loops
	cpx #$00
	beq end
loop:	lda #$00
	ldy #$00
.repeat 8,cline
	sta (_line_addr),y
	iny
.endrepeat
	lda _line_addr
	clc
	adc #$08
	bne no_carry
	inc _line_addr+1
no_carry:
	sta _line_addr
	dex
	bne loop
end:	
	rts
.endproc
