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

    ; 22, 29, 78, 84, 112, 116

    ;--- TEMP, put a table somewhere
    ldx #20
    lda #109
    sta game_board + (MAP_REAL_WIDTH * 10), x

    ldx #21
    lda #84
    sta game_board + (MAP_REAL_WIDTH * 10), x

    ldx #22
    lda #22
    sta game_board + (MAP_REAL_WIDTH * 10), x

    ldx #20
    lda #112
    sta game_board + (MAP_REAL_WIDTH * 11), x

    ldx #21
    lda #116
    sta game_board + (MAP_REAL_WIDTH * 11), x

    ldx #22
    lda #29
    sta game_board + (MAP_REAL_WIDTH * 11), x

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
; Draw half of a tile. It will consider the tile from objects layer if it's non-zero,
; otherwise it will use the background tile
; @y = read offset
; @tmp4 = board_data
; @tmp5 = game_board
;-----------------------------------------------------------------------------------------
drawTopTile:
    ;--- Top Left
    lda [tmp5], y
    tax
    lda tileset1_data, x
    cmp #0
    bne drawTopLeftDone
    lda [tmp4], y
    tax
    lda tileset1_data, x
drawTopLeftDone:
    sta $2007
    ;--- Top Right
    lda [tmp5], y
    tax
    lda tileset2_data, x
    cmp #0
    bne drawTopRightDone
    lda [tmp4], y
    tax
    lda tileset2_data, x
drawTopRightDone:
    sta $2007
    rts

drawBottomTile:
    ;--- Top Left
    lda [tmp5], y
    tax
    lda tileset3_data, x
    cmp #0
    bne drawBottomLeftDone
    lda [tmp4], y
    tax
    lda tileset3_data, x
drawBottomLeftDone:
    sta $2007
    ;--- Top Right
    lda [tmp5], y
    tax
    lda tileset4_data, x
    cmp #0
    bne drawBottomRightDone
    lda [tmp4], y
    tax
    lda tileset4_data, x
drawBottomRightDone:
    sta $2007
    rts

drawTopTileDef:
    ;--- Top Left
    lda [tmp5], y
    tax
    lda tileset1_data, x
    cmp #0
    bne drawTopLeftDoneDef
    lda [tmp4], y
    tax
    lda tileset1_data, x
drawTopLeftDoneDef:
    jsr ppu_Draw
    ;--- Top Right
    lda [tmp5], y
    tax
    lda tileset2_data, x
    cmp #0
    bne drawTopRightDoneDef
    lda [tmp4], y
    tax
    lda tileset2_data, x
drawTopRightDoneDef:
    jsr ppu_Draw
    rts

drawBottomTileDef:
    ;--- Top Left
    lda [tmp5], y
    tax
    lda tileset3_data, x
    cmp #0
    bne drawBottomLeftDoneDef
    lda [tmp4], y
    tax
    lda tileset3_data, x
drawBottomLeftDoneDef:
    jsr ppu_Draw
    ;--- Top Right
    lda [tmp5], y
    tax
    lda tileset4_data, x
    cmp #0
    bne drawBottomRightDoneDef
    lda [tmp4], y
    tax
    lda tileset4_data, x
drawBottomRightDoneDef:
    jsr ppu_Draw
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
    LOAD_ADDR game_board, tmp5

    ldx tmp8
    FillRow_incBoard:
        cpx #0
        beq FillRow_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillRow_incBoard
    FillRow_incBoardDone:

    ldy #0
    FillRow_loopXTop:
        jsr drawTopTile
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXTop
    ldy #0
    FillRow_loopXBottom:
        jsr drawBottomTile
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
    LOAD_ADDR game_board, tmp5

    ldx tmp8
    FillRow_incBoard2:
        cpx #0
        beq FillRow_incBoardDone2
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillRow_incBoard2
    FillRow_incBoardDone2:

    lda #(SCREEN_WIDTH)
    ADD_TO_ADDR tmp4
    lda #(SCREEN_WIDTH)
    ADD_TO_ADDR tmp5

    ldy #0
    FillRow_loopXTop2:
        jsr drawTopTile
        iny ; loop logic
        cpy #SCREEN_WIDTH
        bne FillRow_loopXTop2
    ldy #0
    FillRow_loopXBottom2:
        jsr drawBottomTile
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

    LOAD_ADDR board_data, tmp4
    LOAD_ADDR game_board, tmp5

    ldx tmp8
    FillTopRowDeferred_incBoard:
        beq FillTopRowDeferred_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillTopRowDeferred_incBoard
    FillTopRowDeferred_incBoardDone:

    ldy #0
    FillTopRowDeferred_loopX:
        jsr drawTopTileDef
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
        jsr drawTopTileDef
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

    LOAD_ADDR board_data, tmp4
    LOAD_ADDR game_board, tmp5

    ldx tmp8
    FillBottomRowDeferred_incBoard:
        beq FillBottomRowDeferred_incBoardDone
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp4
        lda #MAP_REAL_WIDTH
        ADD_TO_ADDR tmp5
        dex
        jmp FillBottomRowDeferred_incBoard
    FillBottomRowDeferred_incBoardDone:

    ldy #0
    FillBottomRowDeferred_loopX:
        jsr drawBottomTileDef
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
        jsr drawBottomTileDef
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
    ldx #CAM_NEXT_WAY_POINT_DELAY
    stx game_lastCamCornerTimer
    lda game_lastCamCorner
    asl A
    tax
    lda AnimCamPos, x
    tay
    lda AnimCamPos + 1, x
    tax
    tya
    jsr SetCameraPosAnimate
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

;-----------------------------------------------------------------------------------------
; Some constants things
;-----------------------------------------------------------------------------------------
AnimCamPos: .db 0, 0, 0, 128, 224, 128, 224, 0
CAM_NEXT_WAY_POINT_DELAY = 90
