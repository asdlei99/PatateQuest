    .bank 0

;-----------------------------------------------------------------------------------------
; Load a section at a specific screen in it
;-----------------------------------------------------------------------------------------
tiles_loadSection:
    jsr tiles_updateTileData
    
    PUSH_ALL
    
    lda #$FF
    bit game_zone_dir
    bne tiles_loadSection_doVertical

    ; We will draw (zone * 16 - 8) to (zone * 16 + 23)
    ; In other words, a screen (16 columns) with extra
    ; 8 columns on each sides
    lda game_zone_screen    ; Column * 16
    asl A
    asl A
    asl A
    asl A
    tax
    
    clc                                     ; Add 24
    adc #24
    cmp game_zone_size                      ; Clamp to zone size - 1
    bmi tiles_loadSection_skipClampSize
    lda game_zone_size
tiles_loadSection_skipClampSize:
    sta tmp7
    
    txa                                     ; Subtract 8            
    clc
    adc #-8
    cmp #248                                ; Clamp to 0
    bcc tiles_loadSection_skipClampToZero
    lda #0
tiles_loadSection_skipClampToZero:
    tax
    
    ; Loop the columns
tiles_loadSection_loopColumns:
    txa
    jsr drawColumn
    inx
    txa
    cmp tmp7
    bne tiles_loadSection_loopColumns
    
    ; Set initial scroll position
    lda game_zone_screen
    sta tmp1 + 1
    lda #0
    sta tmp1
    jsr ppu_SetScrolling
    
    lda #SCROLL_DIR_HORIZONTAL()    ; Scrolling direction
    jsr ppu_SetScrollDir
    POP_ALL
    rts
    
tiles_loadSection_doVertical:
    ; We will draw (zone * 13 - 6) to (zone * 13 + 6)
    ; In other words, a screen (13 row) with extra
    ; 6 rows up and down
    lda game_zone_screen    ; Row * 13
    tay
    asl A       ; * 12
    asl A
    asl A
    sta tmp1
    tya
    asl A       ; * 4
    asl A
    clc
    adc tmp1
    sta tmp1
    tya
    clc
    adc tmp1
    tax
    
    clc                                     ; Add 19
    adc #19
    cmp game_zone_size                      ; Clamp to zone size - 1
    bmi tiles_loadSection_VskipClampSize
    lda game_zone_size
tiles_loadSection_VskipClampSize:
    sta tmp7
    
    txa                                     ; Subtract 6           
    clc
    adc #-6
    cmp #248                                ; Clamp to 0
    bcc tiles_loadSection_VskipClampToZero
    lda #0
tiles_loadSection_VskipClampToZero:
    tax
    
    ; Loop the Rows
tiles_loadSection_loopRows:
    txa
    jsr drawRow
    inx
    txa
    cmp #13;tmp7
    bne tiles_loadSection_loopRows
    
    ; Set initial scroll position
    lda game_zone_screen
    sta tmp1 + 1
    lda #0
    sta tmp1
    jsr ppu_SetScrolling

    lda #SCROLL_DIR_VERTICAL()      ; Scrolling direction
    jsr ppu_SetScrollDir
    POP_ALL
    rts
    
;-----------------------------------------------------------------------------------------
; Copies a 4 color palette
;-----------------------------------------------------------------------------------------
copyPalette:
    lda [tmp1], y
    asl A
    asl A
    tay
    lda worldPalettes, y
    sta game_zone_palette, x
    iny
    inx
    lda worldPalettes, y
    sta game_zone_palette, x
    iny
    inx
    lda worldPalettes, y
    sta game_zone_palette, x
    iny
    inx
    lda worldPalettes, y
    sta game_zone_palette, x
    inx
    rts

;-----------------------------------------------------------------------------------------
; Update zone addr
;-----------------------------------------------------------------------------------------
tiles_updateTileData:
    PUSH_ALL
    
    lda game_zone
    asl A                       ; Each zone offset is 2 bytes
    tay
    LOAD_ADDR worldData, tmp1   ; Setup the offset to the zone's first column
    lda [tmp1], y               ; Store the offset at $02
    sta tmp2
    iny
    lda [tmp1], y
    sta tmp2 + 1
    ADDW tmp1, tmp1, tmp2       ; Add base addr with offset
    
    ldy #0
    lda [tmp1], y               ; Load zone properties
    sta game_zone_dir
    lda #$FF
    bit game_zone_dir
    bne tiles_updateTileData_doVertical
    ldy #1
    lda [tmp1], y
    asl A   ; * 16
    asl A
    asl A
    asl A
    sta game_zone_size
    jmp tiles_updateTileData_doneSize
    
tiles_updateTileData_doVertical: ; * 13
    ldy #1
    lda [tmp2], y
    tay
    asl A       ; * 12
    asl A
    asl A
    sta tmp2
    tya
    asl A       ; * 4
    asl A
    clc
    adc tmp2
    sta tmp2
    tya
    clc
    adc tmp2
    sta game_zone_size

tiles_updateTileData_doneSize:
    ldy #4                      ; Copy zone palette
    ldx #0
    jsr copyPalette
    ldy #5 
    jsr copyPalette
    ldy #6
    jsr copyPalette
    ldy #7
    jsr copyPalette
    
    lda #8                      ; Skip header
    ADD_TO_ADDR tmp1
    
    lda tmp1                    ; Store our data pointer
    sta game_zone_pData
    lda tmp1 + 1
    sta game_zone_pData + 1
        
    LOAD_ADDR tiles, game_zone_pTiles0   ; Tileset addr for this zone
    
    lda game_zone_pTiles0
    sta game_zone_pTiles1
    lda game_zone_pTiles0 + 1
    sta game_zone_pTiles1 + 1
    lda #240
    ADD_TO_ADDR game_zone_pTiles1

    lda game_zone_pTiles1
    sta game_zone_pTiles2
    lda game_zone_pTiles1 + 1
    sta game_zone_pTiles2 + 1
    lda #240
    ADD_TO_ADDR game_zone_pTiles2

    lda game_zone_pTiles2
    sta game_zone_pTiles3
    lda game_zone_pTiles2 + 1
    sta game_zone_pTiles3 + 1
    lda #240
    ADD_TO_ADDR game_zone_pTiles3

    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; Get the address of a column
; @a = column ID
; tmp1 will have tile data address
; tmp2 will have vram tile address
; tmp3 will have vram attributes address
;-----------------------------------------------------------------------------------------
getColumnAddr:
    ; Tile data is at:
    ; Column * 16
    pha                     
    clc                     ; a /= 16
    and #$F0
    ror A
    ror A
    ror A
    ror A
    adc game_zone_pData + 1 ; Start by saving the pointer to ROM data
    sta tmp1 + 1
    pla
    
    pha
    asl A
    asl A
    asl A
    asl A
    clc
    adc game_zone_pData
    sta tmp1
    lda tmp1 + 1
    adc #0
    sta tmp1 + 1
    lda #$20
    sta tmp8
    lda #$23
    sta tmp8 + 1
    lda #%00010000
    sta tmp2
    pla
    
    bit tmp2
    beq getColumnAddr_skipOffPage
    pha
    lda #$24
    sta tmp8
    lda #$27
    sta tmp8 + 1
    pla
    
getColumnAddr_skipOffPage:
    and #$0F
    pha                     ; Nametable
    asl A                   ; Every 2 tiles offset
    ora #%10000000          ; +128 because We ignore the 4 first rows for status bar
    sta tmp2
    lda tmp8
    sta tmp2 + 1
    pla
  
    pha
    and #%00000001
    bne getColumnAddr_bottomAttributes
    
    pla
    clc                     ; Attributes
    ror A
    clc
    adc #$C8
    sta tmp3
    lda tmp8 + 1
    sta tmp3 + 1
    rts
    
getColumnAddr_bottomAttributes:
    pla
    clc                     ; Attributes
    ror A
    clc
    adc #$E0
    sta tmp3
    lda tmp8 + 1
    sta tmp3 + 1
    rts
     
;-----------------------------------------------------------------------------------------
; Get the address of a row
; @a = row ID
; tmp1 will have tile data address
; tmp2 will have vram tile address
; tmp3 will have vram attributes address
;-----------------------------------------------------------------------------------------
getRowAddr:
    PUSH_ALL
    
    tay
    lda game_zone_pData
    sta tmp1
    lda game_zone_pData + 1
    sta tmp1 + 1
    tya
    
    ; vram row = (a % 30) * 2 * 32
getRowAddr_mod30_loop:          ; a %= 30
    cmp #30
    bcc getRowAddr_mod30_done
    clc
    sbc #30
    jmp getRowAddr_mod30_loop
getRowAddr_mod30_done:
    tay
    asl A                       ; a *= 64; tmp2 = (8a + 8a + 8a + 8a) + (8a + 8a + 8a + 8a)
    asl A
    asl A
    sta tmp3
    ldx #0
    stx tmp3 + 1
    tax
    ADD_TO_ADDR tmp3
    txa
    ADD_TO_ADDR tmp3
    txa
    ADD_TO_ADDR tmp3
    ADDW tmp2, tmp3, tmp3       ; tmp2 = 2 * tmp3
    lda tmp2 + 1
    ora #$20
    sta tmp2 + 1
    tya
   
    POP_ALL
    rts
   
;-----------------------------------------------------------------------------------------
; Draw a row
; $00 = row address
;-----------------------------------------------------------------------------------------
drawRow:
    jsr getRowAddr
    
    PUSH_ALL
    
    bit $2002               ; read PPU status to reset the high/low latch
    lda tmp2 + 1
    sta $2006
    lda tmp2
    sta $2006
    
    ldy #0
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
    
    iny
    lda [tmp1], y
    sta $2007
   
    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; Draw a column
; $00 = column address
;-----------------------------------------------------------------------------------------
drawColumn:
    jsr getColumnAddr

    PUSH_ALL
   
    bit $2002               ; read PPU status to reset the high/low latch
    ldy #0
    ldx #0
vLoop:
    lda tmp2 + 1            ; Write GPU address then increment 32 for next row
    sta $2006
    lda tmp2
    sta $2006
    lda #32
    ADD_TO_ADDR tmp2

    txa                     ; Write our 2 tiles
    tay
    lda [tmp1], y
    tay
    lda [game_zone_pTiles0], y
    sta $2007
    lda [game_zone_pTiles1], y
    sta $2007
    
    lda tmp2 + 1            ; Write GPU address then increment 32 for next row
    sta $2006 
    lda tmp2
    sta $2006
    lda #32
    ADD_TO_ADDR tmp2

    lda [game_zone_pTiles2], y           ; Write the 2 next tiles
    sta $2007
    lda [game_zone_pTiles3], y
    sta $2007

    inx
    cpx #13
    bne vLoop               ; LOOP
    
    txa
    tay
    
    lda tmp3 + 1            ; Write GPU address then increment 32 for next row (Attributes row)
    sta $2006
    lda tmp3
    sta $2006
    
    lda [tmp1], y
    sta $2007
    
    lda #8
    ADD_TO_ADDR tmp3
    lda tmp3 + 1
    sta $2006
    lda tmp3
    sta $2006
    
    iny
    lda [tmp1], y
    sta $2007
    
    lda #8
    ADD_TO_ADDR tmp3
    lda tmp3 + 1
    sta $2006
    lda tmp3
    sta $2006
    
    iny
    lda [tmp1], y
    sta $2007
    
    tay
    lda #$01
    bit tmp7
    bne drawColumn_skipBottomRow
    
    tya                 ; Bottom row is repeated previous row
    ror A
    ror A
    ror A
    ror A
    pha
    lda #8
    ADD_TO_ADDR tmp3
    lda tmp3 + 1
    sta $2006
    lda tmp3
    sta $2006
    pla
    sta $2007

drawColumn_skipBottomRow:
    POP_ALL
    rts
