CC = gcc
CFLAGS = -W -Wall -g

all: harness

check:
	sh run.sh

harness: harness.o parse.o
	$(CC) $(CFLAGS) -o harness $^

harness.o: parse.h harness.c

parse.o: parse.h parse.c parse-part.c

parse-part.c: test.pacc
	../pacc -o parse-part.c $^
