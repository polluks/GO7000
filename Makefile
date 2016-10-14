CC= cc65:bin/cl65
#CFLAGS= -S -T -O

7: 7.c bins.s gpu.o
	$(CC) $(CFLAGS) 7.c bins.s
	list $@
