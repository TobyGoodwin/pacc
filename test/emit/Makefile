CC = gcc
CFLAGS = -g -W -Wall -I../..

OBJS = ../../arg.o ../../emit.o ../../error.o ../../load.o ../../main.o ../../resolve.o ../../sugar.o ../../syntax.o ../../template.o

emitter: emitter.o
	$(CC) $(CFLAGS) -o $@ $^ $(OBJS)

parse.c: emitter
	./emitter -o $@ Makefile

harness.o: parse.h harness.c

harness: parse.o harness.o
	$(CC) $(CFLAGS) -o $@ $^
