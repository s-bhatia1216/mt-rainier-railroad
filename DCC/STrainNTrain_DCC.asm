; EchoDCC aka Echo 5.0

    org $E000          ; Start of program RAM

;-------------------------------------------
; Constants
;-------------------------------------------

ACIA_DATA    = $8000
ACIA_STATUS  = $8001
ACIA_COMMAND = $8002
ACIA_CONTROL = $8003
DISP = $4000

init	sei

	cld

	ldx #$ff

	txs

	LDA #$05	    	; Group Number 1 - edit to indicate your group number
	STA DISP
	JSR DELAY
;-------------------------------------------
; Initialize 6551 ACIA
;-------------------------------------------

    LDA #$00            ; Master reset (writing to status clears IRQ)
    STA ACIA_STATUS

    LDA #$1E            ; 9600 baud, 8N1 (example value)
    STA ACIA_CONTROL

    LDA #%00001001      ; Enable RX interrupt, no parity, no echo
    STA ACIA_COMMAND

    CLI                 ; Enable interrupts
;-------------------------------------------
; Initialize 6522 VIA
;-------------------------------------------

	lda #$00

	sta $a000

	lda #$fb

	sta $a002

	ldx #$00

	lda #$00

	tay

loop 	inx

	bne loop

	iny
	
	bne loop



; Start of something

	lda $a000

	and #%11111110

	sta $a000

	ora #%00001000
	
	sta $a000

	and #%11110110

	sta $a000

	clc

	lda $a000

	and #%00000100

	beq round

	sec

round	ror a

	tay

	ldx #$07

loadlp	jsr loadacc

	tya

	ror a

	tay

	dex

	beq endload

	bne loadlp

endload sta DISP

	sta $0200

	lda $0200

	beq clrwrt

	and #%00001111

	beq sthwrt

	lda $0200

	and #%11110000

	beq nthwrt

	bne nswrt

clrwrt	jsr clr

	jmp loop

sthwrt	jsr sth

	jmp loop

nthwrt	jsr nth

	jmp loop

nswrt	jsr ns

	jmp loop

loadacc	lda $a000

	and #%11111110

	sta $a000

	ora #%00000001

	sta $a000

	and #%11111110

	sta $a000

	lda $a000

	and #%00000100

	beq loadclr

	sec

	rts

loadclr clc

	rts
	
clr	ldx #$08

strc	jsr strclr

	dex

	beq clrend

	bne strc

clrend	rts

sth	ldx #$08

strs	jsr strsth

	dex

	beq sthend

	bne strs

sthend	rts

nth	ldx #$08

strn	jsr strnth

	dex

	beq nthend

	bne strn

nthend	rts

ns	ldx #$08

strx	jsr strns

	dex

	beq nsend

	bne strx

nsend	rts

strclr	lda $a000

	and #%11111100

	ora valclr,x

	ora #%00100000

	sta $a000

	ora #%00000001

	sta $a000

	rts


valclr  db %00000000

	db %00000000

	db %00000000

	db %00000010

	db %00000000

	db %00000010

	db %00000010

	db %00000000

	db %00000010

strsth	lda $a000

	and #%11111100

	ora valsth,x

	ora #%00100000

	sta $a000

	ora #%00000001

	sta $a000

	rts


valsth  db %00000000

	db %00000000

	db %00000010

	db %00000010

	db %00000000

	db %00000010

	db %00000000

	db %00000010

	db %00000010

strnth	lda $a000

	and #%11111100

	ora valnth,x

	ora #%00100000

	sta $a000

	ora #%00000001

	sta $a000

	rts


valnth  db %00000000

	db %00000010

	db %00000000

	db %00000000

	db %00000010

	db %00000010

	db %00000010

	db %00000000

	db %00000010

strns	lda $a000

	and #%11111100

	ora valns,x

	ora #%00100000

	sta $a000

	ora #%00000001

	sta $a000

	rts


valns   db %00000000

	db %00000010

	db %00000010

	db %00000000

	db %00000010

	db %00000010

	db %00000000

	db %00000010

	db %00000010

DELAY:
	INX
	BNE DELAY
	INY
	BNE DELAY
	RTS

;-------------------------------------------
; IRQ Service Routine
;-------------------------------------------

IRQ_HANDLER:
    PHA
    TXA
    PHA
    TYA
    PHA

; Check if RX data ready
    LDA ACIA_STATUS
    AND #%00001000      ; Mask RX ready bit
    BEQ IRQ_EXIT

; Read received byte
    LDX ACIA_DATA

; Wait until TX empty
WAIT_TX:
    LDA ACIA_STATUS
    AND #%00010000
    BEQ WAIT_TX

; Echo character
	LDX $0200
    STX ACIA_DATA
	STX DISP

IRQ_EXIT:
    PLA
    TAY
    PLA
    TAX
    PLA
	CLI
    RTI


	org $fffc

	dw init
    dw IRQ_HANDLER   ; IRQ vector

	end