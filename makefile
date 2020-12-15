CC=gcc
CFLAGS=-Wall -pedantic
LDFLAGS=-lrt
AUDIOFLAGS=-lpthread -lm -ldl 

all: master.o p1.o p2.o

p1.o: p1.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(AUDIOFLAGS)

p2.o: p2.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) 

master.o: master.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm *.o
