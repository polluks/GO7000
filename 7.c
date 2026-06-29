/* $Id$
 * GO7000 Simulator
 * Dedicated to Doris
 * Credits to Stefan Egger, Jonathan Bowen
 */

#include <stdio.h>
#include <stdlib.h>
#include "gpu.h"
#define XORSWAP_UNSAFE(a, b)    ((a)^=(b),(b)^=(a),(a)^=(b))
#define INT_FRQ 32

extern char rom[];
char iram[0x40];
char xram[0x80];
unsigned short pc;
char opcode;
char sp=0x08;
char a;
char c;
char ac;
char f0;
char f1;
char bs;
char i;
char t;
char p[3];
char psw;

const char VER[]="$\x56\x45\x52: go7000 0.8 ("__DATE__") $";

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
		{
		char v=iram[opcode&0x07];
		unsigned t=(unsigned char)a+(unsigned char)v;
		c=t>>8; ac=((a&15)+(v&15))>>4; a=t;
		}
		goto inc1;

	/* add */
	case 0x03:
		{
		unsigned t=(unsigned char)a+(unsigned char)rom[pc+1];
		c=t>>8; ac=((a&15)+(rom[pc+1]&15))>>4; a=t;
		}
		goto inc2;

	/* outl bus,a */
	case 0x02:
		p[0]=a;
		goto inc1;

	/* ins a,bus */
	case 0x08:
		a=p[0];
		goto inc1;

	/* ins a,p */
	case 0x09:
	case 0x0A:
		a=p[opcode&0x03];
		goto inc1;

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

	/* jb* */
	case 0x12:
	case 0x32:
	case 0x52:
	case 0x72:
	case 0x92:
	case 0xB2:
	case 0xD2:
	case 0xF2:
		pc=a&1<<(opcode>>5) ? (pc&0xff00)+rom[pc+1] : pc+2;
		break;

	/* jc */
	case 0xF6:
		pc=c ? (pc&0xff00)+rom[pc+1] : pc+1;
		break;

	/* jf0 */
	case 0xB6:
		pc=f0 ? (pc&0xff00)+rom[pc+1] : pc+1;
		break;

	/* jf1 */
	case 0x76:
		pc=f1 ? (pc&0xff00)+rom[pc+1] : pc+1;
		break;

	/* mov */
	case 0x23:
		a=rom[pc+1];
		goto inc2;

	/* clr */
	case 0x27:
		a=0;
		goto inc1;

	/* clr */
	case 0x97:
		c=0;
		goto inc1;

	/* clr */
	case 0x85:
		f0=0;
		goto inc1;

	/* clr */
	case 0xA5:
		f1=0;
		goto inc1;

	/* cpl */
	case 0x37:
		a=~a;
		goto inc1;

	/* cpl c */
	case 0xA7:
		c=!c;
		goto inc1;

	/* cpl f0 */
	case 0x95:
		f0=!f0;
		goto inc1;

	/* cpl f1 */
	case 0xB5:
		f1=!f1;
		goto inc1;

	/* mov */
	case 0xA0:
	case 0xA1:
		iram[iram[opcode&0x01]]=a;
		goto inc1;

	/* movp */
	case 0xA3:
		a=rom[pc&0x00|a];
		goto inc1;

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
		goto inc1;

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
		goto inc2;

	/* mov */
	case 0xF0:
	case 0xF1:
		a=iram[iram[opcode&0x07]];
		goto inc1;

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
		goto inc1;

	/* movx */
	case 0x80:
	case 0x81:
		{
		char ad=iram[opcode&0x07];
		a=p[1]&0x08 ? xram[ad] : vdc_read(ad);
		}
		goto inc1;

	/* movx */
	case 0x90:
	case 0x91:
		{
		char ad=iram[opcode&0x07];
		if (p[1]&0x08)
			xram[ad]=a;
		else
			vdc_write(ad, a);
		}
		goto inc1;

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
		goto inc1;

	/* dec */
	case 0x07:
		--a;
		goto inc1;

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
		goto inc1;

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
		goto inc1;

	/* jnz */
	case 0x96:
		pc=a ? (pc&0xff00)+rom[pc+1] : pc+2;
		break;

	/* jz */
	case 0xC6:
		pc=a==0 ? (pc&0xff00)+rom[pc+1] : pc+2;
		break;

	/* rl */
	case 0xE7:
		c=(a&0x80)?1:0;
		a=(a<<1)|c;
		goto inc1;

	/* rr */
	case 0x77:
		c=a&0x01;
		a=(a>>1)|(c<<7);
		goto inc1;

	/* rlc */
	case 0xF7:
		{
		char old_c=c;
		c=(a&0x80)?1:0;
		a=(a<<1)|old_c;
		}
		goto inc1;

	/* rrc */
	case 0x67:
		{
		char old_c=c;
		c=a&0x01;
		a=(a>>1)|(old_c<<7);
		}
		goto inc1;

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
		goto inc1;

	/* orl */
	case 0x43:
		a|=rom[pc+1];
		goto inc2;

	/* orl */
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
		p[opcode&0x03]|=rom[pc+1];
		goto inc2;

	/* swap */
	case 0x47:
		a=(a<<4)|(a>>4);
		goto inc1;

	/* outl */
	case 0x39:
	case 0x3A:
		p[opcode&0x03]=a;
		goto inc1;

	/* anl */
	case 0x53:
		a&=rom[pc+1];
		goto inc2;

	/* anl */
	case 0x98:
	case 0x99:
	case 0x9A:
		p[opcode&0x03]&=rom[pc+1];
		goto inc2;

	/* ret */
	case 0x83:
		pc=(iram[--sp]<<8)+iram[--sp];
		break;

	/* retr */
	case 0x93:
		pc=(iram[--sp]<<8)+iram[--sp];
		break;

	/* nop */
	case 0x00:
		goto inc1;

	/* sel mb */
	case 0xC5:
	case 0xD5:
		goto inc1;

	/* sel rb */
	case 0xE5:
		bs=0;
		goto inc1;

	case 0xF5:
		bs=1;
		goto inc1;

	/* dis */
	case 0x15:
		i=0;
		goto inc1;

	/* en */
	case 0x05:
		i=1;
		goto inc1;
	
	/* ent0 */
	case 0x75:
		t=1;
		goto inc1;

	/* strt t */
	case 0x55:
		goto inc1;

	/* strt cnt */
	case 0x45:
		goto inc1;

	/* stop tcnt */
	case 0x65:
		goto inc1;
	
	/* xch */
	case 0x28:
	case 0x29:
	case 0x2A:
	case 0x2B:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:
		XORSWAP_UNSAFE(a, iram[opcode&0x07]);
		goto inc1;

	/* xrl */
	case 0xD0:
	case 0xD1:
		a^=iram[opcode&0x01];
		goto inc1;

	/* xrl */
	case 0xD3:
		a^=rom[pc+1];
		goto inc2;

	/* xrl */
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
		a^=iram[opcode&0x07];
		goto inc1;

	default:
		abort();
	}
inc1:	++pc; return;
inc2:	pc+=2; return;
}

void main(int argc, char *argv[])
{
short i, max=2000;

if (argc==2)
	max=atoi(argv[1]);
double_width();
vdc_init();
for (i=0; i<max; ++i)
	{
	opcode=rom[pc];
	f1=i%INT_FRQ;
	printf("%2x %3x %2d r0=%x r1=%x\n", opcode, pc, sp, iram[0], iram[1]);
	decode();
	psw=(c<<7)|(ac<<6)|(f0<<5)|(bs<<4)|0x08|((sp-8)>>1);
	}
}
