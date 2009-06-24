CC = gcc
CFLAGS = -g -W -Wall

all: pacc ab pr rd hash-test foo

OBJS = emit.o main.o mktree.o syntax.o

pacc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

#pacc.c: paccman.c pacc.peg
#	./pacc pacc.peg;  cp paccman.c pacc.c

emit.o: syntax.h

foo: foo.o
	$(CC) $(LDFLAGS) $< -o $@

foo.o: foo.c gen.c
	$(CC) $(CFLAGS) -c foo.c

mktree.o: mktree.c mk-target.c syntax.h
	$(CC) $(CFLAGS) -c mktree.c

syntax.o: syntax.c syntax.h

gen.c: pacc
	./pacc > gen.c
