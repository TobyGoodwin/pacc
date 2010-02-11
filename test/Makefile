CC = gcc
CFLAGS = -W -Wall -g -I..

all: harness

check:
	sh run.sh

emitter: emitter.o
	$(CC) $(CFLAGS) -o $@ $^ ../arg.o ../emit.o ../error.o ../load.o ../main.o ../resolve.o ../sugar.o ../syntax.o ../template.o

harness: harness.o parse.o
	$(CC) $(CFLAGS) -o harness $^

harness.o: parse.h harness.c

parse.o: parse.h parse.c

parse.c: parse.pacc
	../pacc2 $^
