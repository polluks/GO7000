/* $Id$
 * GO7000
 * Dedicated to Stefan Egger
 */

#include <stdio.h>

extern char mem[];
unsigned int pc;
char opcode;
char reg[8];

const char VER[]="$ver: go7000 0.2 ("__DATE__") $";

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
	pc=((opcode&0xf0)<<3)+mem[pc+1];
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
	pc+=2;
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
opcode=mem[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=mem[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=mem[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=mem[pc];
printf("%x %x\n", opcode, pc);
decode();
opcode=mem[pc];
printf("%x %x\n", opcode, pc);
decode();
}
