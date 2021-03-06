;
; Startup code for cc65 (C64 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         zerobss, callmain
	.import	     	pushax
        .import         BSOUT
        .import         __INTERRUPTOR_COUNT__
        .import         __RAM_START__, __RAM_SIZE__	; Linker generated

	.import		_cgetc, _puts, _memcpy

	.import		__DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__

        .include        "zeropage.inc"
        .include        "c64.inc"

; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

	.word	Start           ;Cold start
	.word	$FEBC		;Warm start, default calls NMI exit.
	.byt	$C3,$C2,$CD,$38,$30 ;magic to identify cartridge

Start:

	jsr	$FF84		;Init. I/O
	jsr	$FF87		;Init. RAM
	jsr	$FF8A		;Restore vectors
	jsr	$FF81		;Init. screen

	; Set up the stack.
	;	lda    	#<(__HIRAM_START__ + __HIRAM_SIZE__)
	lda    	#<($D000)	
	sta	sp
	;	lda	#>(__HIRAM_START__ + __HIRAM_SIZE__)
	lda	#>($D000)
       	sta	sp+1   		; Set argument stack ptr

; If we have IRQ functions, chain our stub into the IRQ vector

;        lda     #<__INTERRUPTOR_COUNT__
;      	beq	NoIRQ1
;      	lda	IRQVec
;       	ldx	IRQVec+1
;      	sta	IRQInd+1
;      	stx	IRQInd+2
;      	lda	#<IRQStub
;      	ldx	#>IRQStub
;      	sei
;      	sta	IRQVec
;      	stx	IRQVec+1
;      	cli

NoIRQ1:
	lda	#<__DATA_RUN__
	ldx	#>__DATA_RUN__
	jsr	pushax
	lda	#<__DATA_LOAD__
	ldx	#>__DATA_LOAD__
	jsr	pushax
	lda	#<__DATA_SIZE__
	ldx	#>__DATA_SIZE__
	jsr	_memcpy

; Switch to second charset

	lda	#14
	jsr	BSOUT

; Call module constructors

        jsr     initlib

; Clear the BSS data.

        jsr     zerobss

; Push the command-line arguments; and, call main().
;ra:
;	inc	$D021
;	jmp	ra

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  jsr	donelib


; Reset the IRQ vector if we chained it.

;        ;pha  			; Save the return code on stack
;	lda     #<__INTERRUPTOR_COUNT__
;	beq	NoIRQ2
;	lda	IRQInd+1
;	ldx	IRQInd+2
;	sei
;	sta	IRQVec
;	stx	IRQVec+1
;	cli


NoIRQ2:
;	lda	#<exitmsg
;	ldx	#>exitmsg
;	jsr	_puts
;	jsr	_cgetc
	jmp	64738		;Kernal reset address as best I know it.

;.rodata
;exitmsg:
;	.byt	"Your program has ended.  Press any key",13,"to continue...",0

