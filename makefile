CC=gcc
CFLAGS=-I.
DEPS = vanimake.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

vanimake: vanimain.o 
	$(CC) -o vanimake vanimain.o 
