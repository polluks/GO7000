CC= cc65:bin/cl65

7: 7.c bins.s gpu.o
	$(CC) 7.c bins.s
	list $@
