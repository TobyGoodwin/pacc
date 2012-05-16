CC = gcc
CFLAGS = -W -Wall -g -I..

all: harness

check:
	sh run.sh

clean:
	rm -f harness harness.o parse.c

emitter: emitter.o
	$(CC) $(CFLAGS) -o $@ $^ ../arg.o ../cook.o ../emit.o ../error.o ../load.o ../main.o ../preen.o ../sugar.o ../syntax.o ../template.o

harness: harness.o parse.o
	$(CC) $(CFLAGS) -o harness $^

feeder: feeder.o partial.o parsefeed.o
	$(CC) $(CFLAGS) -o feeder $^

harness.o: parse.h harness.c

parse.o: parse.h parse.c

parse.c: parse.pacc
	../pacc2 $^

parsefeed.c partial.c: parsefeed.pacc
	../pacc2 -fpartial.c $^
