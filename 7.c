/* GO7000
 * Dedicated to Stefan Egger
 */

#include <stdio.h>

extern char mem[];
int pc;
char opcode;

const char VER[]="$ver: go7000 0.1 ("__DATE__") $";

void decode()
{
switch(opcode)
	{
	case '\x04':
	case '\x24':
	case '\x44':
	case '\x64':
	case '\x84':
	case '\xA4':
	case '\xC4':
	case '\xE4':
	pc=(opcode&0xf0)<<3+mem[pc+1];
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
}
