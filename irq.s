;
; irq.s - Raster IRQ sprite multiplexer for C64
;
; Installs a raster IRQ handler that cycles through up to 4 batches
; of 8 sprites each, enabling up to 32 virtual sprite objects per frame.
;
; Data structures (defined in display.c):
;   mux_entries[4]  - array of 4 batch entries, each 34 bytes
;   mux_triggers[4] - raster trigger line for each batch
;   mux_count        - number of batches
;   mux_current      - current batch index
;
; Entry layout (struct mux_entry, 34 bytes):
;   +0  8 bytes  Y positions for VIC $D001, $D003, ..., $D00F
;   +8  8 bytes  X positions for VIC $D000, $D002, ..., $D00E
;   +16 1 byte   X MSB for VIC $D010
;   +17 1 byte   Sprite enable for VIC $D015
;   +18 8 bytes  Colors for VIC $D027-$D02E
;   +26 8 bytes  Pointers for $07F8-$07FF
;   =34 total

.export _mux_install

.import _mux_entries
.import _mux_triggers
.import _mux_count
.import _mux_current

; Zero-page temp pointer ($70-$71, safe for cc65 user code)
MPTR    = $70
MPTRHI  = MPTR + 1

.segment "CODE"

_mux_install:
    sei
    lda #<mux_handler
    sta $0314
    lda #>mux_handler
    sta $0315
    lda #$01
    sta $D01A
    cli
    rts

; Address table for batch entries (4 batches, 34 bytes each)
batch_addr:
    .addr _mux_entries
    .addr _mux_entries + 34
    .addr _mux_entries + 68
    .addr _mux_entries + 102

mux_handler:
    pha
    txa
    pha
    tya
    pha

    lda $D019
    and #$01
    bne irq_ok
    jmp exit
irq_ok:
    sta $D019

    ldx _mux_current
    cpx _mux_count
    bcc process
    jmp done
process:

    txa
    asl a
    tax
    lda batch_addr, x
    sta MPTR
    lda batch_addr+1, x
    sta MPTR+1

    ; Y positions (+0) -> $D001, $D003, ...
    ldy #0
    lda (MPTR),y
    sta $D001
    iny
    lda (MPTR),y
    sta $D003
    iny
    lda (MPTR),y
    sta $D005
    iny
    lda (MPTR),y
    sta $D007
    iny
    lda (MPTR),y
    sta $D009
    iny
    lda (MPTR),y
    sta $D00B
    iny
    lda (MPTR),y
    sta $D00D
    iny
    lda (MPTR),y
    sta $D00F

    ; X positions (+8) -> $D000, $D002, ...
    ldy #8
    lda (MPTR),y
    sta $D000
    iny
    lda (MPTR),y
    sta $D002
    iny
    lda (MPTR),y
    sta $D004
    iny
    lda (MPTR),y
    sta $D006
    iny
    lda (MPTR),y
    sta $D008
    iny
    lda (MPTR),y
    sta $D00A
    iny
    lda (MPTR),y
    sta $D00C
    iny
    lda (MPTR),y
    sta $D00E

    ; X MSB (+16)
    ldy #16
    lda (MPTR),y
    sta $D010

    ; Sprite enable (+17)
    ldy #17
    lda (MPTR),y
    sta $D015

    ; Colors (+18) -> $D027-$D02E
    ldy #18
    lda (MPTR),y
    sta $D027
    iny
    lda (MPTR),y
    sta $D028
    iny
    lda (MPTR),y
    sta $D029
    iny
    lda (MPTR),y
    sta $D02A
    iny
    lda (MPTR),y
    sta $D02B
    iny
    lda (MPTR),y
    sta $D02C
    iny
    lda (MPTR),y
    sta $D02D
    iny
    lda (MPTR),y
    sta $D02E

    ; Pointers (+26) -> $07F8-$07FF
    ldy #26
    lda (MPTR),y
    sta $07F8
    iny
    lda (MPTR),y
    sta $07F9
    iny
    lda (MPTR),y
    sta $07FA
    iny
    lda (MPTR),y
    sta $07FB
    iny
    lda (MPTR),y
    sta $07FC
    iny
    lda (MPTR),y
    sta $07FD
    iny
    lda (MPTR),y
    sta $07FE
    iny
    lda (MPTR),y
    sta $07FF

    inc _mux_current
    ldx _mux_current
    cpx _mux_count
    bcs done

    lda _mux_triggers, x
    sta $D012
    jmp exit

done:
    lda #$00
    sta $D01A

exit:
    pla
    tay
    pla
    tax
    pla
    rti
