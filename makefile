CC=gcc
CFLAGS=-Wall -pedantic
LDFLAGS=-lrt

all: master.o p1.o p2.o

p1.o: p1.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

p2.o: p2.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

master.o: master.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm *.o
