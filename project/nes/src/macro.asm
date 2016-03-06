    .macro LOAD_ADDR
    lda #LOW(\1)
    sta \2
    lda #HIGH(\1)
    sta \2 + 1
    .endm
	
    .macro ADDW
    lda \3
    clc
    adc \2
    sta \1
    lda \3 + 1
    adc \2 + 1
    sta \1 + 1
    .endm
    
    .macro ADD_TO_ADDR
    clc
    adc \1
    sta \1
    lda \1 + 1
    adc #0
    sta \1 + 1
    .endm

    .macro PUSH_ALL
    pha
    txa
    pha
    tya
    pha
    .endm
    
    .macro POP_ALL
    pla
    tay
    pla
    tax
    pla
    .endm

    .macro PUSH_Y
    pha
    tya
    pha
    .endm
    
    .macro POP_Y
    pla
    tay
    pla
    .endm

    .macro PUSH_X
    pha
    txa
    pha
    .endm
    
    .macro POP_X
    pla
    tax
    pla
    .endm

;-----------------------------------------------------------------------------------------
; Animates a variable from X to A with an EASY_OUT type anim.
; Useful for camera animation.
;-----------------------------------------------------------------------------------------
EaseOut2:
    stx tmp8
    tya
    cmp tmp8
    beq EaseOut2_done
    bcc EaseOut2_greater
    tya
    sec
    sbc tmp8
    and #%11110000
    beq EaseOut2_lowSkip1
    inx
EaseOut2_lowSkip1:
    and #%11000000
    beq EaseOut2_lowSkip2
    inx
EaseOut2_lowSkip2:
    and #%10000000
    beq EaseOut2_lowSkip3
    inx
EaseOut2_lowSkip3:
    inx
    rts
EaseOut2_greater:
    sty tmp8
    txa
    sec
    sbc tmp8
    and #%11110000
    beq EaseOut2_greaterSkip1
    dex
EaseOut2_greaterSkip1:
    and #%11000000
    beq EaseOut2_greaterSkip2
    dex
EaseOut2_greaterSkip2:
    and #%10000000
    beq EaseOut2_greaterSkip3
    dex
EaseOut2_greaterSkip3:
    dex
EaseOut2_done:
    rts

    .macro EASE_OUT
    ldx \1 ; from
    ldy \2 ; to
    jsr EaseOut2
    stx \1
    .endm
