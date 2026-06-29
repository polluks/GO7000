# VDC features in VIC

The i8244 VDC register file `vdc[256]` is the primary interface between the 8048 and the video/sound chip. The 8048 writes registers via MOVX when `P1.3=0`, reads via MOVX when `P1.3=0`.

## Register map (current)

| Addr | Purpose | Status |
|------|---------|--------|
| 0x00 | Sprite 3 Y | routed |
| 0x01 | Sprite 3 X | routed |
| 0x02 | Sprite 3 attr | routed |
| 0x04 | Sprite 2 Y | routed |
| 0x05 | Sprite 2 X | routed |
| 0x06 | Sprite 2 attr | routed |
| 0x08 | Sprite 1 Y | routed |
| 0x09 | Sprite 1 X | routed |
| 0x0A | Sprite 1 attr | routed |
| 0x0C | Sprite 0 Y | routed |
| 0x0D | Sprite 0 X | routed |
| 0x0E | Sprite 0 attr | routed |
| 0x10-0x17 | Color palette (8× 4b fg+4b bg) | unmapped |
| 0x30 | Display control (blank/scroll/mode) | unmapped |
| 0x40 | Tone 1 freq lo | → SID v1.freq.L |
| 0x41 | Tone 1 freq hi | → SID v1.freq.H |
| 0x42 | Tone 1 volume/ctrl | → SID v1.ctrl |
| 0x43 | Tone 2 freq lo | → SID v2.freq.L |
| 0x44 | Tone 2 freq hi | → SID v2.freq.H |
| 0x45 | Tone 2 volume/ctrl | → SID v2.ctrl |
| 0x46 | Noise freq | → SID v3.freq.L |
| 0x47 | Noise ctrl | → SID v3.ctrl |
| 0x80-0x9F | Sprite pattern SRAM (32 bytes, 4×8) | routed |
| 0xA0-0xFF | Video/sprite attribute RAM (256 nybbles) | unmapped |

## Existing data

- **Font**: `cset[512]` — 64 chars × 8 bytes, monochrome 8×8 bitmaps in `gpu.c:15`
- **Double-width**: `csetram` at `0x3800` — expanded 16-wide glyphs built by `double_width()`
- **Sprite RAM**: `sram[32]` — 4 banks of 8 bytes loaded from VDC 0x80-0x9F
- **Sprite state**: `spr_y[4]`, `spr_x[4]`, `spr_attr[4]`
- **Sound**: `struct __sid` updated on VDC writes 0x40-0x47

## Missing — character layer rendering

The BIOS writes screen characters into the VDC/VIC video RAM starting at VDC offset 0xA0 (or via an external screen buffer accessible through the VDC). To render:

1. Walk 40 columns × 24 rows of screen RAM (mapped in VDC 0xA0+)
2. For each cell, read the character index and foreground/background colour nybbles from attribute RAM (0x80-0x9F mapping or a separate colour RAM region)
3. Look up the 8-byte glyph in `cset[512]` (or `csetram` if double width is enabled)
4. Plot the 8×8 (or 16×8) pixels into a framebuffer using the palette
5. Output as PPM (plain P6 PPM works trivially in sim65)

## Missing — sprite compositing

After the character layer is drawn, composite the 4 sprites over it:

1. For each sprite, check if `(x, y)` is within the visible area
2. Read its 8 bytes from `sram[bank*8 … bank*8+7]`
3. For each bit set, write the sprite colour from `spr_attr[i]` at `(spr_x[i] + bit, spr_y[i] + row)`
4. Collision detection: if a pixel was already non-zero, set a collision flag

## Missing — colour

Real i8244 colour registers (0x10-0x17) hold up to 8 palette entries of 4-bit foreground + 4-bit background. Map these to 24-bit RGB for PPM output using the VIC-II palette (commonly accepted as the "VIC colours"):

| Index | Colour      | R    G    B
|-------|-------------|-----------------
| 0     | Black       |  0    0    0
| 1     | White       |255  255  255
| 2     | Red         |136    0    0
| 3     | Cyan        |170  255  238
| 4     | Purple      |204   68  204
| 5     | Green       |  0  204   85
| 6     | Blue        |  0    0  170
| 7     | Yellow      |238  238  119
| 8     | Orange      |221  136   85
| 9     | Brown       |102   68    0
|10     | Pink        |255  119  119
|11     | Dark Grey   | 51   51   51
|12     | Grey        |119  119  119
|13     | Light Green |170  255  102
|14     | Light Blue  |  0  136  255
|15     | Light Grey  |187  187  187

If no palette is written, default to VIC colours 0 (black background) and 1 (white foreground).

```c
static const unsigned char vic_pal[16][3] = {
    {  0,  0,  0}, {255,255,255}, {136,  0,  0}, {170,255,238},
    {204, 68,204}, {  0,204, 85}, {  0,  0,170}, {238,238,119},
    {221,136, 85}, {102, 68,  0}, {255,119,119}, { 51, 51, 51},
    {119,119,119}, {170,255,102}, {  0,136,255}, {187,187,187},
};
```

## Missing — display control

VDC register 0x30 controls blanking, scroll offset, and character/attribute mode. A simple renderer should check:
- Bit 7 (blank): skip rendering
- Bits 0-3 (scroll): vertical scroll offset

## Running with sim65

sim65 has no display output, so the framebuffer must be written to a PPM file after `max` cycles:

```c
// in main(), after the emulation loop:
FILE *f = fopen("frame.ppm", "wb");
fprintf(f, "P6\n%d %d\n255\n", W, H);
fwrite(fb, 1, W*H*3, f);
fclose(f);
```

For double-buffered animation, dump a frame every N cycles or on vertical blank.
