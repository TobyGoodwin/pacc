CC = gcc
CFLAGS = -g -W -Wall

all: pacc ab pr rd hash-test foo

OBJS = emit.o error.o main.o resolve.o sugar.o syntax.o

MANOBJS = $(OBJS) paccman.o

paccman: $(MANOBJS) 
	$(CC) $(LDFLAGS) -o $@ $(MANOBJS)

PACCOBJS = $(OBJS) pacc.o

pacc: $(PACCOBJS)
	$(CC) $(LDFLAGS) -o $@ $(PACCOBJS)

pacc.c: paccman pacc.pacc
	IFS= ./paccman `cat pacc.pacc` > pacc.c

emit.o: syntax.h

foo: foo.o emit.o syntax.o error.o resolve.o sugar.o
	$(CC) $(LDFLAGS) $^ -o $@

foo.o: foo.c gen.c
	$(CC) $(CFLAGS) -c foo.c

mktree.o: mktree.c syntax.h
	$(CC) $(CFLAGS) -c mktree.c

syntax.o: syntax.c syntax.h

gen.c: pacc
	./pacc > gen.c
