CC = gcc
CFLAGS = -W -Wall -g

all: check

check: harness

harness: harness.o parse.o
	$(CC) $(CFLAGS) -o harness $^

parse.o: parse.c parse-part.c

parse-part.c: baf0.pacc
	../pacc -o parse-part.c $^
