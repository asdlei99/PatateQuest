;-----------------------------------------------------------------------------------------
; iNES header
;-----------------------------------------------------------------------------------------
    .inesprg 2   ; 2x 16KB PRG code
    .ineschr 1   ; 1x 8KB CHR data
    .inesmir 1   ; background mirroring
    .inesmap 0   ; mapper 0 = NROM, no bank swapping
    	
;-----------------------------------------------------------------------------------------
; Interrupts
;-----------------------------------------------------------------------------------------
	.bank 3
	.org $FFFA     ;first of the three vectors starts here
	.dw NMI        ;when an NMI happens (once per frame if enabled) the processor will jump to the label NMI:
	.dw RESET      ;when the processor first turns on or is reset, it will jump to the label RESET:
	.dw 0          ;external interrupt IRQ is not used in this engine

;-----------------------------------------------------------------------------------------
; CHR banks
;-----------------------------------------------------------------------------------------
	.bank 4
	.org $0000
	.incbin "assets/pattern.chr"
	.incbin "assets/sprites.chr"

;-----------------------------------------------------------------------------------------
; Game code bank
;-----------------------------------------------------------------------------------------
	.bank 0
	.org $8000 
    
    .rsset $0000
tmp1    .rs 2 ; $00-$01
tmp2    .rs 2 ; $02-$03
tmp3    .rs 2 ; $04-$05
tmp4    .rs 2 ; $06-$07
tmp5    .rs 2 ; $08-$09
tmp6    .rs 2 ; $0A-$0B
tmp7    .rs 2 ; $0C-$0D
tmp8    .rs 2 ; $0E-$0F

;-----------------------------------------------------------------------------------------
; Turn ON, Reset. Start of the program
;-----------------------------------------------------------------------------------------
	.bank 0
RESET:
	sei          ; disable IRQs
	cld          ; disable decimal mode
	ldx #$40
	stx $4017    ; disable APU frame IRQ
	ldx #$FF
	txs          ; Set up stack
	inx          ; now X = 0
	stx $2000    ; disable NMI
	stx $2001    ; disable rendering
	stx $4010    ; disable DMC IRQs
	jsr WaitVBlank
clrmem:
	lda #$00
	sta $0000, x
	sta $0100, x
	sta $0200, x
	sta $0400, x
	sta $0500, x
	sta $0600, x
	sta $0700, x
	lda #$FE
	sta $0300, x
	inx
	bne clrmem
	jsr WaitVBlank

	ldx #$10			; Wait couple frames
	jsr Wait			;---
	jmp Main

;-----------------------------------------------------------------------------------------
; Includes
;-----------------------------------------------------------------------------------------
    .include "src/macro.asm"
    .include "src/ppu.asm"
    .include "src/timing.asm"

;-----------------------------------------------------------------------------------------
; Main loop
;-----------------------------------------------------------------------------------------
	.bank 0
Main:
    jsr ppu_On
    jsr OnInit
MainLoop:
    jsr ppu_Submit
    jsr OnFrame
    jmp MainLoop

;-----------------------------------------------------------------------------------------
; Lastly, includes our game code
;-----------------------------------------------------------------------------------------
    .include "src/game.asm"
