#ifdef __C64__

#include <_vic2.h>
#define VIC (*(struct __vic2*)0xD000)

#include "gpu.h"

/* Sprite data RAM at VIC-visible address */
#define SPRDATA ((char*)0x2000)
#define SPR_SLOTS 32

/* Multplexer data shared with assembly IRQ handler (irq.s).
 * struct mux_entry: 34 bytes per batch */
struct mux_entry {
	char vic_y[8];
	char vic_x[8];
	char vic_x_msb;
	char vic_ena;
	char vic_col[8];
	char vic_ptr[8];
};

struct mux_entry mux_entries[4];
char mux_triggers[4];
char mux_count;
char mux_current;

extern void mux_install(void);

/* Sprite pool for Y-sorting */
static char pooly[32];
static char poolx[32];
static char poolcol[32];
static char *poolsrc[32];

static void bitmap_to_spr(const char *src, char *dst)
{
	char r;
	for (r = 0; r < 21; r++) {
		if (r < 8) {
			dst[r * 3] = src[r];
			dst[r * 3 + 1] = 0;
		} else {
			dst[r * 3] = 0;
			dst[r * 3 + 1] = 0;
		}
		dst[r * 3 + 2] = 0;
	}
}

void display_init(void)
{
	int i;
	VIC.bordercolor = 0;
	VIC.bgcolor0 = 0;
	VIC.spr_ena = 0;
	for (i = 0; i < SPR_SLOTS * 64; i++) SPRDATA[i] = 0;
	mux_install();
}

void vdc_render(void)
{
	char i, j, n, t, y, x, pat, cbyte, en;
	char *tmp;
	unsigned cset_idx;
	char start;

	en = vdc[0xA0];
	n = 0;

	/* Collect 4 O2 sprites */
	if (en & 4) {
		for (i = 0; i < 4; i++) {
			if (spr_y[i] > 0 && spr_y[i] < 250) {
				pooly[n] = spr_y[i];
				poolx[n] = spr_x[i];
				poolcol[n] = spr_attr[i] & 0x0F;
				poolsrc[n] = sram + i * 8;
				n++;
			}
		}
	}

	/* Collect 12 foreground chars (0x10-0x3F) */
	if (en & 2) {
		for (i = 0; i < 12; i++) {
			y = vdc[0x10 + i * 4];
			x = vdc[0x10 + i * 4 + 1];
			pat = vdc[0x10 + i * 4 + 2];
			cbyte = vdc[0x10 + i * 4 + 3];
			if (y > 0 && y < 250) {
				pooly[n] = y;
				poolx[n] = x;
				poolcol[n] = (cbyte >> 1) & 0x0F;
				cset_idx = ((unsigned char)cbyte & 1) << 8 | (unsigned char)pat;
				cset_idx &= 0x1FF;
				poolsrc[n] = cset + cset_idx;
				n++;
			}
		}
	}

	/* Collect 16 quad chars (0x40-0x7F) */
	if (en & 8) {
		for (i = 0; i < 16; i++) {
			y = vdc[0x40 + i * 4];
			x = vdc[0x40 + i * 4 + 1];
			pat = vdc[0x40 + i * 4 + 2];
			cbyte = vdc[0x40 + i * 4 + 3];
			if (y > 0 && y < 250) {
				pooly[n] = y;
				poolx[n] = x;
				poolcol[n] = (cbyte >> 1) & 0x0F;
				cset_idx = ((unsigned char)cbyte & 1) << 8 | (unsigned char)pat;
				cset_idx &= 0x1FF;
				poolsrc[n] = cset + cset_idx;
				n++;
			}
		}
	}

	/* Bubble sort pool by Y (ascending) */
	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			if (pooly[j] < pooly[i]) {
				t = pooly[i]; pooly[i] = pooly[j]; pooly[j] = t;
				t = poolx[i]; poolx[i] = poolx[j]; poolx[j] = t;
				t = poolcol[i]; poolcol[i] = poolcol[j]; poolcol[j] = t;
				tmp = poolsrc[i]; poolsrc[i] = poolsrc[j]; poolsrc[j] = tmp;
			}
		}
	}

	/* Pre-convert all sprite bitmaps to VIC-II format */
	for (i = 0; i < n; i++)
		bitmap_to_spr(poolsrc[i], SPRDATA + i * 64);

	/* Build multiplexer batches (up to 4 batches of 8) */
	mux_count = 0;
	mux_current = 0;
	start = 0;

	while (start < n) {
		char batch_size = n - start;
		char s;
		char x_msb;
		struct mux_entry *e;

		if (batch_size > 8) batch_size = 8;

		e = &mux_entries[mux_count];

		x_msb = 0;
		for (s = 0; s < batch_size; s++) {
			e->vic_y[s] = pooly[start + s];
			e->vic_x[s] = poolx[start + s];
			e->vic_col[s] = poolcol[start + s];
			e->vic_ptr[s] = ((unsigned)SPRDATA) / 64 + start + s;
		}

		/* Fill unused sprite slots with off-screen Y */
		for (s = batch_size; s < 8; s++) {
			e->vic_y[s] = 255;
			e->vic_x[s] = 0;
			e->vic_col[s] = 0;
			e->vic_ptr[s] = 0;
		}

		e->vic_x_msb = x_msb;
		e->vic_ena = (1 << batch_size) - 1;

		/* Set trigger for this batch transition (first batch set by C) */
		mux_triggers[mux_count] = 0;
		if (mux_count > 0) {
			char first_y = pooly[start];
			mux_triggers[mux_count] = (first_y > 8) ? first_y - 8 : 0;
		}

		mux_count++;
		start += batch_size;
	}

	/* Apply batch 0 immediately */
	{
		struct mux_entry *e0 = &mux_entries[0];
		char s;
		VIC.spr_ena = 0;
		for (s = 0; s < 8; s++) {
			if (e0->vic_ena & (1 << s)) {
				VIC.spr_pos[s].x = e0->vic_x[s];
				VIC.spr_pos[s].y = e0->vic_y[s];
				VIC.spr_color[s] = e0->vic_col[s];
				*(volatile char*)(0x07F8 + s) = e0->vic_ptr[s];
				VIC.spr_ena |= 1 << s;
			}
		}
		*(volatile char*)0xD010 = e0->vic_x_msb;
	}

	/* Set up raster IRQ for remaining batches */
	if (mux_count > 1) {
		char next = mux_triggers[1];
		mux_current = 1;
		*(volatile char*)0xD012 = next;
		*(volatile char*)0xD01A = 1;
	} else {
		*(volatile char*)0xD01A = 0;
	}

	/* Background and grid colours */
	VIC.bgcolor0 = vdc[0xA1] & 0x0F;
	if (en & 1)
		VIC.bordercolor = vdc[0xA3] & 0x0F;
	else
		VIC.bordercolor = 0;
}

#else /* sim65 stub */

void vdc_render(void) { }

#endif
