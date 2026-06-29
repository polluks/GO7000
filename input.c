#include <_6526.h>

struct __6526 cia;

void input_init()
{
cia.pra = 0xFF;
cia.prb = 0xFF;
cia.ddra = 0;
cia.ddrb = 0;
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
