CC= cc65:bin/cl65
SIM= cc65:bin/sim65
CFLAGS= -tsim6502 -lT:7.lst #-S -T -O

7: 7.c bins.o gpu.o
	$(CC) $(CFLAGS) 7.c bins.o gpu.o
	list $@

gpu.o: gpu.c gpu.h


run: 7
	$(SIM) $?
