MAP_WIDTH = 30
MAP_REAL_WIDTH = 32
MAP_HEIGHT = 23
SCREEN_WIDTH = 16
SCREEN_HEIGHT = 15
SCREEN_CENTER_X = 8
SCREEN_CENTER_Y = 7

GAME_STATE_SHOWING_MAP = 0
GAME_STATE_LOADING_SECTION = 1
GAME_STATE_PLAY = 2

TILE_VISIBLE_BIT = %10000000
TILE_PASSABLE_LESS_THAN = 20
TILE_ID_BITS = %01111111

    .rsset $0010
game_state: .rs 1 ; x10 Game state
game_UNUSED: .rs 1 ; x11 
game_camera: .rs 2 ; x12
game_cameraAnim: .rs 2 ; x14
game_lastRowLoaded: .rs 1 ; x15
game_lastCamCorner: .rs 1 ; x16
game_lastCamCornerTimer: .rs 1 ; x17

    .rsset $0400
game_board:             .rs (MAP_REAL_WIDTH * MAP_HEIGHT); 32 * 23 tiles

    .bank 0
    ;.include "src/tiles.asm"

;-----------------------------------------------------------------------------------------
; Copies the board static data to our map data in preparation for level loading
;-----------------------------------------------------------------------------------------
ClearBoard:
    PUSH_ALL
    ldx #(MAP_REAL_WIDTH * MAP_HEIGHT / 8)
	lda #$00
    clrmboard:
	sta game_board, x
	sta game_board + 92, x
	sta game_board + 92 * 2, x
	sta game_board + 92 * 3, x
	sta game_board + 92 * 4, x
	sta game_board + 92 * 5, x
	sta game_board + 92 * 6, x
	sta game_board + 92 * 7, x
	dex
	bne clrmboard

    ;--- TEMP, put a table somewhere
    ldx #8
    lda #100
    sta game_board + (MAP_REAL_WIDTH * 8), x

    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; Get the address of a row
; @a = row ID
; tmp2 will have vram tile address
; tmp3 will have vram attributes address
;-----------------------------------------------------------------------------------------
getRowAddr:
    ; vram row = (a % 15) * 2 * 32
getRowAddr_mod15_loop:          ; a %= 15
    cmp #15
    bcc getRowAddr_mod15_done
    sec
    sbc #15
    jmp getRowAddr_mod15_loop
getRowAddr_mod15_done:
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
    rts

;-----------------------------------------------------------------------------------------
; Fill both screens with a row
; @x = row index
;-----------------------------------------------------------------------------------------
FillRow:
    PUSH_ALL

    stx tmp8
    txa
    jsr getRowAddr
    bit $2002               ; read PPU status to reset the high/low latch
    lda tmp2 + 1
    sta $2006
    lda tmp2
    sta $2006

    LOAD_ADDR board_data, tmp4

    ldx tmp8
    FillRow_incBoard:
        cpx #0
        beq FillRow_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        dex
        jmp FillRow_incBoard
    FillRow_incBoardDone:

    ldy #0
    FillRow_loopXTop:
        lda [tmp4], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetTop_data, x
        sta $2007
        inx
        lda tilesetTop_data, x
        sta $2007
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXTop
    ldy #0
    FillRow_loopXBottom:
        lda [tmp4], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetBottom_data, x
        sta $2007
        inx
        lda tilesetBottom_data, x
        sta $2007
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXBottom

    lda tmp2 + 1
    clc
    adc #$04
    sta tmp2 + 1
    bit $2002               ; read PPU status to reset the high/low latch
    lda tmp2 + 1
    sta $2006
    lda tmp2
    sta $2006

    LOAD_ADDR board_data, tmp4

    ldx tmp8
    FillRow_incBoard2:
        cpx #0
        beq FillRow_incBoardDone2
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        dex
        jmp FillRow_incBoard2
    FillRow_incBoardDone2:

    lda #(SCREEN_WIDTH)
    ADD_TO_ADDR tmp4

    ldy #0
    FillRow_loopXTop2:
        lda [tmp4], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetTop_data, x
        sta $2007
        inx
        lda tilesetTop_data, x
        sta $2007
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXTop2
    ldy #0
    FillRow_loopXBottom2:
        lda [tmp4], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetBottom_data, x
        sta $2007
        inx
        lda tilesetBottom_data, x
        sta $2007
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXBottom2

    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; Fill both screens with a row, in the command buffer. Deferred at the end of the screen
; @x = row index
;-----------------------------------------------------------------------------------------
FillTopRowDeferred:
    PUSH_Y

    stx tmp8
    txa
    jsr getRowAddr
    lda #32
    jsr ppu_BeginRow

    LOAD_ADDR board_data, tmp5

    ldx tmp8
    FillTopRowDeferred_incBoard:
        beq FillTopRowDeferred_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillTopRowDeferred_incBoard
    FillTopRowDeferred_incBoardDone:

    ldy #0
    FillTopRowDeferred_loopX:
        lda [tmp5], y
        and #TILE_ID_BITS
        clc
        asl A
        tax
        lda tilesetTop_data, x
        jsr ppu_Draw
        inx
        lda tilesetTop_data, x
        jsr ppu_Draw
        iny ; loop logic
        cpy #(SCREEN_WIDTH)
        bne FillTopRowDeferred_loopX

    lda tmp2 + 1
    clc
    adc #$04
    sta tmp2 + 1
    lda #32
    jsr ppu_BeginRow

    FillTopRowDeferred_loopX2:
        lda [tmp5], y
        and #TILE_ID_BITS
        clc
        asl A
        tax
        lda tilesetTop_data, x
        jsr ppu_Draw
        inx
        lda tilesetTop_data, x
        jsr ppu_Draw
        iny ; loop logic
        cpy #(SCREEN_WIDTH * 2)
        bne FillTopRowDeferred_loopX2

    POP_Y
    rts
    
;-----------------------------------------------------------------------------------------
; Fill both screens with a row, in the command buffer. Deferred at the end of the screen
; @x = row index
;-----------------------------------------------------------------------------------------
FillBottomRowDeferred:
    PUSH_Y

    stx tmp8
    txa
    jsr getRowAddr
    lda #32
    ADD_TO_ADDR tmp2
    lda #32
    jsr ppu_BeginRow

    LOAD_ADDR board_data, tmp5

    ldx tmp8
    FillBottomRowDeferred_incBoard:
        beq FillBottomRowDeferred_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillBottomRowDeferred_incBoard
    FillBottomRowDeferred_incBoardDone:

    ldy #0
    FillBottomRowDeferred_loopX:
        lda [tmp5], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetBottom_data, x
        jsr ppu_Draw
        inx
        lda tilesetBottom_data, x
        jsr ppu_Draw
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillBottomRowDeferred_loopX

    lda tmp2 + 1
    clc
    adc #$04
    sta tmp2 + 1
    lda #32
    jsr ppu_BeginRow

    FillBottomRowDeferred_loopX2:
        lda [tmp5], y
        and #TILE_ID_BITS
        asl A
        tax
        lda tilesetBottom_data, x
        jsr ppu_Draw
        inx
        lda tilesetBottom_data, x
        jsr ppu_Draw
        iny ; loop logic
        cpy #(SCREEN_WIDTH * 2)
        bne FillBottomRowDeferred_loopX2

    POP_Y
    rts

;-----------------------------------------------------------------------------------------
; Shows the board on the screen, fresh. Call only this during loading (ppu off)
;-----------------------------------------------------------------------------------------
RefreshScreen:
    PUSH_ALL

    ldx #0
    RefreshScreen_loopRows:
        jsr FillRow
        inx ; loop logic
        cpx #(SCREEN_HEIGHT)
        bne RefreshScreen_loopRows
    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; Sets the desired camera pos. It will animate to that position over time
; @a, @x = position (x, y)
;-----------------------------------------------------------------------------------------
SetCameraPosAnimate:
    sec
    sbc #SCREEN_CENTER_X
    bcs SetCameraPosAnimateX_skipMinClamp
    lda #0
SetCameraPosAnimateX_skipMinClamp:
    cmp #14
    bcc SetCameraPosAnimateX_skipMaxClamp
    lda #14
SetCameraPosAnimateX_skipMaxClamp:
    MUL16
    sta game_camera
    txa
    sec
    sbc #SCREEN_CENTER_Y
    bcs SetCameraPosAnimateY_skipMinClamp
    lda #0
SetCameraPosAnimateY_skipMinClamp:
    cmp #8
    bcc SetCameraPosAnimateY_skipMaxClamp
    lda #8
SetCameraPosAnimateY_skipMaxClamp:
    MUL16
    sta game_camera + 1
    rts

;-----------------------------------------------------------------------------------------
; Initialize method for the game
;-----------------------------------------------------------------------------------------
OnInit:
    jsr ppu_Off ; Begin loading

    ;--- load board
    jsr ClearBoard
    jsr RefreshScreen

    ;--- palettes
    LOAD_ADDR board_pal, tmp1 ; Default palette
    jsr ppu_SetPal0
    LOAD_ADDR board_pal, tmp1
    jsr ppu_SetPal1

    ;--- scroll direction
    lda #SCROLL_DIR_HORIZONTAL
    jsr ppu_SetScrollDir

    jsr ppu_On ; Done loading

    lda #BG_PATTERN0 ; Our background tiles are on the first pattern
    jsr ppu_SetBGPattern

    ;--- Set initial state
    lda #GAME_STATE_SHOWING_MAP
    sta game_state

    rts

;-----------------------------------------------------------------------------------------
; Game main loop. This is called every frame by the engine
;-----------------------------------------------------------------------------------------
    .bank 0
OnFrame:
    ;--- switch (game_state) {
    lda game_state
    cmp #GAME_STATE_SHOWING_MAP
    bne not_GAME_STATE_SHOWING_MAP
    jsr func_GAME_STATE_SHOWING_MAP ; case GAME_STATE_SHOWING_MAP:
    jmp done_GAME_STATE ; break;
not_GAME_STATE_SHOWING_MAP:
    cmp #GAME_STATE_LOADING_SECTION
    bne not_GAME_STATE_LOADING_SECTION
    jsr func_GAME_STATE_LOADING_SECTION ; case GAME_STATE_LOADING_SECTION:
    jmp done_GAME_STATE ; break;
not_GAME_STATE_LOADING_SECTION:
    cmp #GAME_STATE_PLAY
    bne not_GAME_STATE_PLAY
    jsr func_GAME_STATE_PLAY ; case GAME_STATE_PLAY:
    jmp done_GAME_STATE ; break;
not_GAME_STATE_PLAY:
done_GAME_STATE:  ; }
    rts

;-----------------------------------------------------------------------------------------
; This function scrolls the camera.
; When scrolling in Y, tiles need to be added at the bottom or at the top. It gets
; quite complex
;-----------------------------------------------------------------------------------------
UpdateCameraScroll:
    PUSH_ALL
    ;--- Cam before in Y
    lda game_cameraAnim + 1
    sta tmp7 + 1
    DIV16
    sta tmp7

    ;--- Animate the camera, then set the scroll value
    EASE_OUT game_cameraAnim, game_camera
    stx cmd_scrollX
    EASE_OUT game_cameraAnim + 1, game_camera + 1
    stx cmd_scrollY

    ;--- When scrolling up or down we need to update the board tiles
    ;--- First, determine if we move up or down
    lda game_cameraAnim + 1
    cmp tmp7 + 1
    beq scrollDrawingDone
    bcs doPositiveScrollLogic

doNegativeScrollLogic:
    ;--- Bottom row
    lda game_cameraAnim + 1
    DIV16
    cmp tmp7
    beq negSkipDrawBottomRow
    tax
    jsr FillBottomRowDeferred
negSkipDrawBottomRow:

    ;--- Top row
    lda tmp7 + 1
    clc
    adc #8
    DIV16
    sta tmp7
    lda game_cameraAnim + 1
    DIV16
    cmp tmp7
    beq scrollDrawingDone
    ldx tmp7
    jsr FillTopRowDeferred

    jmp scrollDrawingDone

doPositiveScrollLogic:
    ;--- Top row
    lda game_cameraAnim + 1
    clc
    adc #8
    DIV16
    cmp tmp7
    beq skipDrawTopRow
    lda tmp7
    clc
    adc #(SCREEN_HEIGHT)
    tax
    jsr FillTopRowDeferred
skipDrawTopRow:

    ;--- Bottom row
    lda game_cameraAnim + 1
    DIV16
    cmp tmp7
    beq skipDrawBottomRow
    lda tmp7
    clc
    adc #(SCREEN_HEIGHT)
    tax
    jsr FillBottomRowDeferred
skipDrawBottomRow:

scrollDrawingDone:
    POP_ALL
    rts

;-----------------------------------------------------------------------------------------
; This state shows the map with an animation from each corners
;-----------------------------------------------------------------------------------------
func_GAME_STATE_SHOWING_MAP:
    ldx game_lastCamCornerTimer
    beq switchCorner
    dex
    stx game_lastCamCornerTimer
    jmp continueAnim
switchCorner:
    ldx #120
    stx game_lastCamCornerTimer
    lda game_lastCamCorner
    cmp #1
    beq corner2
    cmp #2
    beq corner3
    cmp #3
    beq corner4
corner1:
    lda #0
    ldx #0
    jsr SetCameraPosAnimate
    jmp finishCornerSwitch
corner2:
    lda #0
    ldx #128
    jsr SetCameraPosAnimate
    jmp finishCornerSwitch
corner3:
    lda #224
    ldx #128
    jsr SetCameraPosAnimate
    jmp finishCornerSwitch
corner4:
    lda #224
    ldx #0
    jsr SetCameraPosAnimate
    jmp finishCornerSwitch
finishCornerSwitch:
    ldx game_lastCamCorner
    inx
    txa
    and #%00000011 ; a %= 4
    sta game_lastCamCorner
continueAnim:
    jsr UpdateCameraScroll
    rts

;-----------------------------------------------------------------------------------------
; 
;-----------------------------------------------------------------------------------------
func_GAME_STATE_LOADING_SECTION:
    rts

;-----------------------------------------------------------------------------------------
; 
;-----------------------------------------------------------------------------------------
func_GAME_STATE_PLAY:
    rts

;-----------------------------------------------------------------------------------------
; Include game data
;-----------------------------------------------------------------------------------------
    .include "src/data.asm"
