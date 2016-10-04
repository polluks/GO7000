/* $Id$
 * GO7000
 * Dedicated to Stefan Egger
 */

#include <stdio.h>

extern char rom[];
char ram[0x40];
unsigned int pc;
char opcode;
char sp;

const char VER[]="$ver: go7000 0.3 ("__DATE__") $";

void decode()
{
switch(opcode)
	{
	/* jmp */
	case '\x04':
	case '\x24':
	case '\x44':
	case '\x64':
	case '\x84':
	case '\xA4':
	case '\xC4':
	case '\xE4':
	pc=((opcode&0xf0)<<3)+rom[pc+1];
	break;

	/* mov */
	case '\xB8':
	case '\xB9':
	case '\xBA':
	case '\xBB':
	case '\xBC':
	case '\xBD':
	case '\xBE':
	case '\xBF':
	ram[(opcode&0x0f)>>3]=rom[pc+1];
//printf("%x\n", (opcode&0x0f)>>4);
	pc+=2;
	break;

	/* call */
	case '\x14':
	case '\x34':
	case '\x54':
	case '\x74':
	case '\x94':
	case '\xB4':
	case '\xD4':
	case '\xF4':
	pc=((opcode&0xf0)<<3)+rom[pc+1];
	++sp;
	break;

	/* sel */
	case '\xD5':
	/* nop */
	case '\x00':
	++pc;
	break;
	}
}

void main()
{
opcode=rom[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=rom[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=rom[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=rom[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=rom[pc];
printf("%x %x\n", opcode, pc);
decode();
}
