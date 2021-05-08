CC=gcc 
CFLAGS=-Wpedantic -std=gnu99 -Wall -g

all: simcpu

simcpu: simcpu.c simcpu.h heap.o
	$(CC) $(CFLAGS) simcpu.c heap.o -o simcpu

simcpu.o: simcpu.c simcpu.h heap.o
	$(CC) $(CFLAGS) -c simcpu.c heap.o -o simcpu.o 

heap.o: heap.c simcpu.h
	$(CC) $(CFLAGS) -c heap.c -o heap.o 

clean:
	rm -f *.o simcpu
