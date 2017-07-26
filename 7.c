/* $Id$
 * GO7000 Simulator
 * Dedicated to Doris
 * Credits to Stefan Egger, Jonathan Bowen
 */

#include <stdio.h>
#include <stdlib.h>
#include "gpu.h"

extern char rom[];
char iram[0x40];
char xram[0x80];
unsigned short pc;
char opcode;
char sp=0x08;
char a;
char c;
char i;
char p[2];

const char VER[]="$\x56\x45\x52: go7000 0.7 ("__DATE__") $";

void decode()
{
switch(opcode)
	{
	/* add */
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	a+=iram[opcode&0x07];
	++pc;
	break;

	/* add */
	case 0x03:
	a+=rom[pc+1];
	pc+=2;
	break;

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

	/* jbn */
	case 0x12:
	case 0x32:
	case 0x52:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
	pc=a&opcode>>4 ? (pc&0xff00)+rom[pc+1] : pc+1;
	break;

	/* jc */
	case 0xF6:
	pc=c ? (pc&0xff00)+rom[pc+1] : pc+1;
	break;

	/* mov */
	case 0x23:
	a=rom[pc+1];
	pc+=2;
	break;

	/* clr */
	case 0x27:
	a=0;
	++pc;
	break;

	/* clr */
	case 0x97:
	c=0;
	++pc;
	break;

	/* cpl */
	case 0x37:
	a=~a;
	++pc;
	break;

	/* mov */
	case 0xA0:
	case 0xA1:
	iram[iram[opcode&0x01]]=a;
	++pc;
	break;

	/* movp */
	case 0xA3:
	a=rom[pc&0x00|a];
	++pc;
	break;

	/* mov */
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	iram[opcode&0x07]=a;
	++pc;
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
	iram[opcode&0x07]=rom[pc+1];
	pc+=2;
	break;

	/* mov */
	case 0xF8:
	case 0xF9:
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF:
	a=iram[opcode&0x07];
	++pc;
	break;

	/* movx */
	case 0x80:
	case 0x81:
	a=xram[opcode&1];
	++pc;
	break;

	/* movx */
	case 0x90:
	case 0x91:
	xram[opcode&1]=a;
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
	iram[sp++]=pc+2;
	iram[sp++]=pc+2>>8;
	pc=((opcode&0x60)<<3)+rom[pc+1];
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
	++iram[opcode&0x07];
	++pc;
	break;

	/* dec */
	case 0x07:
	--a;
	++pc;
	break;

	/* dec */
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	--iram[opcode&0x07];
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
	--iram[opcode&0x07];
	pc=iram[opcode&0x07] ? (pc&0xff00)+rom[pc+1] : pc+2;
	break;

	/* inc */
	case 0x17:
	++a;
	++pc;
	break;

	/* jnz */
	case 0x96:
	pc=a ? (pc&0xff00)+rom[pc+1] : pc+2;
	break;

	/* jz */
	case 0xC6:
	pc=a==0 ? (pc&0xff00)+rom[pc+1] : pc+2;
	break;

	/* rlc */
	case 0xF7:
	a<<=a;// TODO
	++pc;
	break;

	/* rrc */
	case 0x67:
	a>>=a;// TODO
	++pc;
	break;

	/* orl */
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F:
	a|=iram[opcode&0x07];
	++pc;
	break;

	/* orl */
	case 0x43:
	a|=rom[pc+1];
	pc+=2;
	break;

	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
//
	pc+=2;
	break;

	/* anl */
	case 0x53:
	a&=rom[pc+1];
	pc+=2;
	break;

	/* anl */
	case 0x98:
	case 0x99:
	p[opcode&1]&=rom[pc+1];
	pc+=2;
	break;

	/* ret */
	case 0x83:
	pc=(iram[--sp]<<8)+iram[--sp];
	break;
	
	/* sel */
	case 0xC5:
	case 0xD5:
	/* nop */
	case 0x00:
	++pc;
	break;

	/* dis */
	case 0x15:
	i=0;
	++pc;
	break;

	/* en */
	case 0x05:
	i=1;
	++pc;
	break;
	
	/* xch */
	case 0x28:
	case 0x29:
	case 0x2A:
	case 0x2B:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
	a=iram[opcode&0x07];// TODO
	++pc;
	break;

	default:
	abort();
	}
}

void main()
{
short i;

double_width();
for (i=0; i<1500; ++i)
	{
	opcode=rom[pc];
	printf("%2x %3x %2d r0=%x r1=%x\n", opcode, pc, sp, iram[0], iram[1]);
	decode();
	}
}
