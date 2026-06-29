#include <_6526.h>

struct __6526 cia;
char keyb[6];

void input_init()
{
char i;
cia.pra = 0xFF;
cia.prb = 0xFF;
cia.ddra = 0;
cia.ddrb = 0;
for (i=0; i<6; ++i) keyb[i] = 0xFF;
}

char input_bus(char p1, char p2)
{
if ((p1 & 0x18) == 0x18)
	return (p2 & 7) == 1 ? cia.pra : cia.prb;
return 0xFF;
}

void input_set(int n, char state)
{
if (n == 0) cia.pra = state;
else cia.prb = state;
}

void input_set_bit(int n, int bit, int pressed)
{
if (n == 0)
	{
	if (pressed) cia.pra &= ~(1 << bit);
	else cia.pra |= (1 << bit);
	}
else
	{
	if (pressed) cia.prb &= ~(1 << bit);
	else cia.prb |= (1 << bit);
	}
}

void input_set_key(char row, char col, char pressed)
{
if (row < 6 && col < 8)
	{
	if (pressed) keyb[row] &= ~(1 << col);
	else keyb[row] |= (1 << col);
	}
}

char input_read_p2(char p1, char p2)
{
if (!(p1 & 4))
	{
	char row = p2 & 7;
	if (row < 6)
		{
		char i, cols = ~keyb[row] & 0xFF;
		if (cols)
			{
			for (i=0; i<8; ++i)
				if (cols & (1 << i))
					return (p2 & 0x0F) | ((i ^ 7) << 5);
			}
		}
	return (p2 & 0x0F) | 0xF0;
	}
return p2 | 0xF0;
}
