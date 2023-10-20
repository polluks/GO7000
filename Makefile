AS= ca65
CC= cl65
SIM= sim65
CFLAGS= -tsim6502 -l/tmp/7.lst #-S -T -O

7: 7.c bins.o gpu.o
	$(CC) $(CFLAGS) 7.c bins.o gpu.o
	ls -l $@

gpu.o: gpu.c gpu.h


check: 7
	$(SIM) -c $? $M

dist:
	tar czf /tmp/GO7000.tgz *
