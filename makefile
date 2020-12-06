CC=gcc
FLAGS=-W

%.o: %.c
	$(CC) -o $@ $<

all: master.o p1.o
