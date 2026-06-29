AS= ca65
CC= cl65
SIM= sim65

# sim65 target (testing/debug)
sim65: 7
7: 7.c bins.s gpu.c input.c display.c
	$(CC) -tsim6502 -O -o $@ $^
	ls -l $@

# c64 target (real hardware)
c64: 7.prg
7.prg: 7.c bins.s gpu.c input.c display.c irq.s
	$(CC) -t c64 -O -o $@ $^
	ls -l $@

check: 7
	$(SIM) -c $< $(M)

dist:
	tar czf /tmp/GO7000.tgz *
