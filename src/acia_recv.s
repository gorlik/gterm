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
	
.export _NMI
.export _ser_recv
	
.segment	"CODE"

;; ****************** NMI Interrupt *********************
;; 59-60 cycles (70 when clearing RTS) + NMI entry 
.proc _NMI: near
	pha 			; 3
 	;;	txa		; 2
	;; 	pha		; 3
	;; save y
	tya 			; 2
	pha			; 3
	
	ldy _put		; 3
	;; interrrupt ack
	lda ACIA_STATUS 	; 4

	;; receive a char
	lda ACIA_DATA 		; 4
	sta _SER_BUF,y		; 4
	iny			; 2
	sty _put		; 3

	;; border color on receive
	;;	sty $d020	; 4

	;; check for buffer full
	;;	clc 		; 2 - this is not exact as it might be 1 off but it's 2 cycles faster
	tya 			; 2
	sbc _get 		; 3
	cmp #$F0 		; 2
	bpl end_nmi 		; 2-4
	
	;; clear ACIA RTS
	lda #$01		; 2
	sta ACIA_COMMAND	; 4
	lda #$05		; 2
	sta _SCR_BASE-$400+38	; 4 - on screen receive "led"

	;; return from interrupt
end_nmi:
	pla			; 4
	tay			; 2
	;;	pla
	;; 	tax
	pla			; 4
	rti			; 6
.endproc


	
;; ------------- polling receive ------------------
.proc _ser_recv: near
	ldx #$F0		; 2
	ldy _put		; 3
	sei			; 2
	
	lda ACIA_STATUS 	; 4 - check if a char is waiting in the buffer
	and #$08		; 2
	beq set_rts		; 2-4
	lda ACIA_DATA 		; 4  
	sta _SER_BUF,y		; 4
	iny			; 2

set_rts:
	;; set RTS
	lda #$01		; 2
	sta _SCR_BASE-$400+38	; 4
	lda #$0B		; 2
	sta ACIA_COMMAND	; 4

;; read loop
;; 23-24 cycles for 1 char receive
loop:
	;; 	inc $d020
	lda ACIA_STATUS 	; 4
	and #$08		; 2
	beq no_recv		; 2-4
	;; receive a char
recv:	
	lda ACIA_DATA 		; 4
	sta _SER_BUF,y		; 4
	iny			; 2
	;; border color on receive
	;;	sty $d020		; 4
	
	;; check for buffer full - no need as we only try to receive up to 256 chars
	;;	clc 			; 2
	;; 	tya 			; 2
	;; 	sbc _get 		; 3
	;; 	cmp #$F0 		; 2
	;; 	bpl last 		; 2
	
no_recv:
	dex			; 2
	bne loop		; 2-4

last:
	;; clear RTS	
	lda #$03		; 2
	sta ACIA_COMMAND	; 4
	lda #$05		; 2
	sta _SCR_BASE-$400+38

	;; check if one more char in the buffer
	lda ACIA_STATUS 	; 4
	and #$08		; 2
	beq end			; 2-4
	lda ACIA_DATA 		; 4
	sta _SER_BUF,y		; 4
	iny			; 2
	
end:	
	sty _put		; 3 - save put pointer
	cli			; 2
	rts			; 6
.endproc
