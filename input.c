#include <_6526.h>

struct __6526 cia;
char keyb[6];
char rstflg;

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

void input_set_nmi()
{
rstflg = 1;
}

void input_set_key(char row, char col, char pressed)
{
if (row < 6 && col < 8)
	{
	if (pressed) keyb[row] &= ~(1 << col);
	else keyb[row] |= (1 << col);
	}
}

void input_char(char c, char pressed)
{
char row = 0xFF, col;
switch (c)
	{
	case '0': row=0; col=0; break;
	case '1': row=0; col=1; break;
	case '2': row=0; col=2; break;
	case '3': row=0; col=3; break;
	case '4': row=0; col=4; break;
	case '5': row=0; col=5; break;
	case '6': row=0; col=6; break;
	case '7': row=0; col=7; break;
	case '8': row=1; col=0; break;
	case '9': row=1; col=1; break;
	case ' ': row=1; col=4; break;
	case '?': row=1; col=5; break;
	case 'l': case 'L': row=1; col=6; break;
	case 'p': case 'P': row=1; col=7; break;
	case '+': row=2; col=0; break;
	case 'w': case 'W': row=2; col=1; break;
	case 'e': case 'E': row=2; col=2; break;
	case 'r': case 'R': row=2; col=3; break;
	case 't': case 'T': row=2; col=4; break;
	case 'u': case 'U': row=2; col=5; break;
	case 'i': case 'I': row=2; col=6; break;
	case 'o': case 'O': row=2; col=7; break;
	case 'q': case 'Q': row=3; col=0; break;
	case 's': case 'S': row=3; col=1; break;
	case 'd': case 'D': row=3; col=2; break;
	case 'f': case 'F': row=3; col=3; break;
	case 'g': case 'G': row=3; col=4; break;
	case 'h': case 'H': row=3; col=5; break;
	case 'j': case 'J': row=3; col=6; break;
	case 'k': case 'K': row=3; col=7; break;
	case 'a': case 'A': row=4; col=0; break;
	case 'z': case 'Z': row=4; col=1; break;
	case 'x': case 'X': row=4; col=2; break;
	case 'c': case 'C': row=4; col=3; break;
	case 'v': case 'V': row=4; col=4; break;
	case 'b': case 'B': row=4; col=5; break;
	case 'm': case 'M': row=4; col=6; break;
	case '.': row=4; col=7; break;
	case '-': row=5; col=0; break;
	case '*': row=5; col=1; break;
	case '/': row=5; col=2; break;
	case '=': row=5; col=3; break;
	case 'y': case 'Y': row=5; col=4; break;
	case 'n': case 'N': row=5; col=5; break;
	case 127: row=5; col=6; break;
	case '\n': case '\r': row=5; col=7; break;
	}
if (row != 0xFF)
	input_set_key(row, col, pressed);
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
