CC=gcc
CFLAGS=-W

%.o: %.c
	$(CC) -o $@ $< -lrt

all: master.o p1.o
