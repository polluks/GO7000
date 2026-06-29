# VDC Feature Mapping to C64 + sim65

The i8244 VDC register file `vdc[256]` is the interface between the 8048 and video/sound. The 8048 writes via MOVX when `P1.3=0`, reads via MOVX when `P1.3=0`.

Two build targets:
- **sim65** (`-t sim65`): 8048 emulation + VDC register writes + SID/CIA updates. No display output.
- **C64** (`-t c64`): Same 8048 logic, but VDC register writes drive the Oscar64 virtual sprite multiplexer, grid rendering, SID, and CIA.

## Register Map

| Addr | Size | Purpose | C64 Mapping |
|------|------|---------|-------------|
| 0x00 | 1 | Sprite 3 Y | vspr sprite slot |
| 0x01 | 1 | Sprite 3 X | vspr sprite slot |
| 0x02 | 1 | Sprite 3 attr (colour+pattern) | vspr colour, sram→VIC data |
| 0x04 | 1 | Sprite 2 Y | vspr sprite slot |
| 0x05 | 1 | Sprite 2 X | vspr sprite slot |
| 0x06 | 1 | Sprite 2 attr | vspr colour, sram→VIC data |
| 0x08 | 1 | Sprite 1 Y | vspr sprite slot |
| 0x09 | 1 | Sprite 1 X | vspr sprite slot |
| 0x0A | 1 | Sprite 1 attr | vspr colour, sram→VIC data |
| 0x0C | 1 | Sprite 0 Y | vspr sprite slot |
| 0x0D | 1 | Sprite 0 X | vspr sprite slot |
| 0x0E | 1 | Sprite 0 attr | vspr colour, sram→VIC data |
| 0x10-0x3F | 48 | 12 foreground char objects (Y,X,ptr,color ×12) | 12 virtual sprites via vspr |
| 0x40-0x7F | 64 | 4 quad groups (4 chars × 4 bytes each) | 16 virtual sprites via vspr |
| 0x80-0x9F | 32 | Sprite pattern SRAM (4 sprites × 8 bytes) | Convert to VIC sprite data |
| 0xA0 | 1 | Display control (enable bits per layer) | Gate vspr/grid updates |
| 0xA1 | 1 | Background colour | VIC border colour |
| 0xA2 | 1 | Sound volume | SID volume register |
| 0xA3 | 1 | Grid colour | VIC colour index |
| 0xC0-0xC8 | 9 | Grid horizontal lines 0-8 (coarse Y) | Rendered in software |
| 0xD0-0xD8 | 9 | Grid horizontal fine Y offset (lines 0-8) | Rendered in software |
| 0xE0-0xE9 | 10 | Grid vertical lines 0-9 (X) | Rendered in software |

### Sound (0x40-0x47) → SID

Already implemented in `gpu.c` — maps tone/noise registers to `struct __sid`:

| VDC Addr | SID Register |
|----------|-------------|
| 0x40-0x41 | Tone 1 freq → v1.freq |
| 0x42 | Tone 1 ctrl → v1.ctrl |
| 0x43-0x44 | Tone 2 freq → v2.freq |
| 0x45 | Tone 2 ctrl → v2.ctrl |
| 0x46 | Noise freq → v3.freq |
| 0x47 | Noise ctrl → v3.ctrl |

Volume (0xA2) → SID volume register.

## Display Control (0xA0)

Bit fields:
- Bit 0: Grid enable
- Bit 1: Foreground char objects enable
- Bit 2: Sprites enable
- Bit 3: Quad groups enable
- Bit 7: Screen blank (0 = blank, 1 = normal)

On C64: these bits gate whether we call `vspr_show`/`vspr_hide` and whether we render the grid each frame.

## Layer Mapping

### 4 Hardware Sprites (0x00-0x0E)

Uses `sram[32]` (0x80-0x9F) for patterns: 4 sprites × 8 bytes, each byte = 1 row, 1 bit/pixel.

On C64: routed through the same vspr multiplexer as everything else. At startup, the 32-byte sram is converted to 4 VIC-II 63-byte sprite blocks (expanded from 8 bytes single-bitplane to 63 bytes interleaved). On each VDC write to 0x80-0x9F, the affected sprite's VIC data is regenerated.

Mapping: sprites occupy vspr slots 0-3 (or 28-31 — configurable).

### 12 Foreground Character Objects (0x10-0x3F)

Each object is 4 bytes:
- byte 0: Y position (0-255)
- byte 1: X position (0-255)
- byte 2: character pattern pointer, low 8 bits of cset byte offset
- byte 3: bit 0 = pointer bit 8 (9th bit of cset offset), bits 4-1 = colour (0-15)

Character shape comes from `cset[512]` at offset `((byte3 & 1) << 8) | byte2`. The 8×8 monochrome bitmap is converted to a VIC-II sprite image (transparent background, colour from byte3 bits 4-1).

On C64: 12 virtual sprites via vspr (slots 4-15). On each VDC write to 0x10-0x3F, the affected sprite's position, colour, or image is updated.

### 4 Quad Groups (0x40-0x7F)

Each quad group = 4 consecutive 4-byte character objects (same format as foreground chars above). So quad at 0x40 defines chars at offsets 0x40, 0x44, 0x48, 0x4C. Total: 16 individual char objects.

On C64: 16 virtual sprites via vspr (slots 16-31). Same conversion as foreground chars.

### Grid (0xC0-0xC8, 0xD0-0xD8, 0xE0-0xE9)

The grid consists of 9 horizontal lines (0xC0-0xC8 for coarse Y, 0xD0-0xD8 for fine Y offset) and 10 vertical lines (0xE0-0xE9 for X position).

| Addr | Purpose |
|------|---------|
| 0xC0 | Horizontal line 0 coarse Y |
| 0xC1 | Horizontal line 1 coarse Y |
| ... | ... |
| 0xC8 | Horizontal line 8 coarse Y |
| 0xD0 | Horizontal line 0 fine Y offset (lower nibble) |
| 0xD1 | Horizontal line 1 fine Y offset |
| ... | ... |
| 0xD8 | Horizontal line 8 fine Y offset |
| 0xE0 | Vertical line 0 X |
| 0xE1 | Vertical line 1 X |
| ... | ... |
| 0xE9 | Vertical line 9 X |

Grid colour from 0xA3 (VIC colour index). On C64, grid lines are rendered as pixels on the VIC-II bitmap/character screen or drawn by a custom routine. Since lines are axis-aligned, each frame we can:
1. Read the 9 horizontal line Y positions (0xC0-0xC8) + fine offsets (0xD0-0xD8)
2. Read the 10 vertical line X positions (0xE0-0xE9)
3. Set the corresponding pixels on screen using grid colour (0xA3)

With 9 horizontal + 10 vertical lines = 19 line segments. Each line is drawn full-width or full-height across a VIC-II bitmap (or character screen).

Alternative: use VIC-II sprite 7 as a grid overlay by generating a sprite image that encodes the line pattern, but updating it every frame may be costly. Pure software rendering into a bitmap stored in character-form memory is simpler.

## Colour Mapping

i8244 uses 4-bit colour indices (0-15) which map directly to VIC-II 16-colour palette:

| Index | VIC Colour | Usage |
|-------|-----------|-------|
| 0-7   | Black→Yellow | Background/char foreground |
| 8-15  | Orange→Light Grey | Foreground char objects (typical) |

The colour field in foreground char byte 3 (bits 4-1) is right-shifted by 1 to get 0-15. So `colour_idx = (byte3 >> 1) & 0xF`.

## Sprite Data Conversion

`cset[512]` is 64 chars × 8 bytes, each byte = 1 row, MSB-left 8-pixel monochrome bitmap. VIC-II sprites use a 63-byte interleaved format (3 colour cells × 21 bytes, 1 bit/pixel with transparency).

Per sprite/char, at startup (or when the cset entry or sram entry changes):
1. Read 8 bytes from `cset[offset]` or `sram[base]`
2. Expand each row: `bit 7 → byte 0 bit 7, bit 6 → byte 0 bit 6, ...`
3. Store in VIC sprite 63-byte format at the sprite pointer location

The 4 O2 sprites (sram[32]) each have their own VIC sprite. The 12 foreground chars share VIC sprite data from cset, but since each char object can point to any of the 64 cset entries, we need 64 pre-converted VIC sprite images, one per `cset[8*n … 8*n+7]`. The 4-bit colour is set per-object via `vspr_color()`.

## Frame Loop (C64)

```c
for (;;) {
    // 1. Run 8048 for one frame's worth of cycles
    cycles = emulate_8048(/* ~20000 cycles = 1/60s */);

    // 2. VDC state is now updated from all MOVX writes

    // 3. Update virtual sprites from VDC register file
    if (vdc[0xA0] & 0x02) { // foreground chars enabled
        for (i = 0; i < 12; i++) {
            if (char_visible(i)) {
                vspr_move(VSPR_FG_BASE + i,
                    vdc[0x10 + i*4 + 1], // X
                    vdc[0x10 + i*4]);    // Y
            } else {
                vspr_hide(VSPR_FG_BASE + i);
            }
        }
    }
    // ... same for quads (VSPR_QUAD_BASE + j) and sprites (VSPR_SPR_BASE + k)

    // 4. Sort virtual sprites by Y
    vspr_sort();

    // 5. Wait for raster IRQ bottom of frame
    rirq_wait();

    // 6. Update multiplexer
    vspr_update();

    // 7. Update SID and CIA
    //   (already done on each VDC write — or flushed here)

    // 8. Draw grid (software) into VIC-II screen RAM / colour RAM

    // 9. Sort raster IRQ list for next frame
    rirq_sort();
}
```

## Frame Loop (sim65)

```c
for (;;) {
    // Just run 8048 cycles — no display output
    cycles = emulate_8048(20000);
    // VDC writes update vdc[] array, and SID/CIA side-effects happen inline
}
```

sim65 target is a stub for testing 8048 code: verifies VDC register writes, sound register updates, and input reads. No PPM, no framebuffer.

## Existing Data Structures

- **`cset[512]`** — 64 chars × 8 bytes, monochrome 8×8 bitmaps (gpu.c)
- **`vdc[256]`** — VDC register file, written by 8048 MOVX, read each frame to drive C64 output
- **`sram[32]`** — 4 sprite patterns, loaded from VDC 0x80-0x9F
- **`spr_y[4]`**, **`spr_x[4]`**, **`spr_attr[4]`** — sprite state derived from 0x00-0x0E
- **`struct __sid`** — SID sound chip, updated on VDC writes 0x40-0x47, 0xA2
- **VIC colour constants** — in gpu.h (VIC_BLACK through VIC_LTGREY)

## Virtual Sprite Layout

vspr slots (configurable via compiler defines):

| Slots | Objects | VSPR_COUNT |
|-------|---------|-----------|
| 0-3   | 4 O2 sprites (sram[32]) | 4 |
| 4-15  | 12 foreground chars | 12 |
| 16-31 | 4 quads × 4 chars each | 16 |
| **Total** | | **32** |

Set `-dVSPRITES_MAX=32 -dNUM_IRQS=32` when building with Oscar64.

## Collision Detection

VIC-II provides hardware sprite collision between C64 sprites but not between virtual sprites. Since the O2's collision model expects collisions between layers (sprite↔sprite, sprite↔character, etc.), these must be calculated in software during the frame loop by comparing bounding boxes of active objects after reading VDC state.
