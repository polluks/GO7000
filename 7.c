/* $Id$
 * GO7000 Simulator
 * Dedicated to Stefan Egger
 */

#include <stdio.h>
#include <stdlib.h>

extern char rom[];
char iram[0x40];
unsigned short pc;
char opcode;
char sp=0x08;
char a;

const char VER[]="$ver: go7000 0.4 ("__DATE__") $";

void decode()
{
switch(opcode)
	{
	/* jmp */
	case 0x04:
	case 0x24:
	case 0x44:
	case 0x64:
	case 0x84:
	case 0xA4:
	case 0xC4:
	case 0xE4:
	pc=((opcode&0xf0)<<3)+rom[pc+1];
	break;

	/* mov */
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	case 0xBC:
	case 0xBD:
	case 0xBE:
	case 0xBF:
	iram[(opcode&0x0f)>>3]=rom[pc+1];
//printf("%x\n", (opcode&0x0f)>>4);
	pc+=2;
	break;

	/* movx */
	case 0x80:
	case 0x81:
	case 0x90:
	case 0x91:
//	iram[(opcode&0x0f)>>3]=rom[pc+1];
	++pc;
	break;

	/* call */
	case 0x14:
	case 0x34:
	case 0x54:
	case 0x74:
	case 0x94:
	case 0xB4:
	case 0xD4:
	case 0xF4:
	iram[sp++]=pc;
	pc=((opcode&0xf0)>>5)+rom[pc+1];
	break;

	/* inc */
	case 0x18:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:
	++iram[(opcode&0x0f)>>3];
	++pc;
	break;

	/* djnz */
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	--iram[(opcode&0x0f)>>3];
	/* jnz */
	case 0x96:
	pc=a ? rom[pc+1] : pc+2;
	break;

	/* ret */
	case 0x83:
	pc=iram[sp--];
	break;
	
	/* sel */
	case 0xD5:
	/* nop */
	case 0x00:
	++pc;
	break;
	
	default:
	abort();
	}
}

void main()
{
short i;
for (i=0; i<10; ++i)
	{
	opcode=rom[pc];
	printf("%x %3x\n", opcode, pc);
	decode();
	}
}
