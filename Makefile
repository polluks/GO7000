CC= cc65:bin/cl65
CFLAGS= -tsim6502 #-S -T -O

7: 7.c bins.s gpu.o
	$(CC) $(CFLAGS) 7.c bins.s
	list $@

run: 7
	cc65:bin/sim65 $?
