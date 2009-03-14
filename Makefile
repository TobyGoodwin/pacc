CFLAGS=-g -W -Wall

CC = gcc
CFLAGS = -g -W -Wall

all: pacc ab pr rd hash-test foo

OBJS = emit.o main.o mktree.o pacc.o syntax.o

pacc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

pacc.c: paccman.c pacc.peg
	./pacc pacc.peg;  cp paccman.c pacc.c

foo.o: foo.c gen.c

gen.c: pacc
	./pacc > gen.c
