CC = gcc
CFLAGS = -g -W -Wall

all: pacc

OBJS = emit.o error.o main.o resolve.o sugar.o syntax.o

OBJS0 = $(OBJS) pacc0.o

pacc0: $(OBJS0) 
	$(CC) $(LDFLAGS) -o $@ $^

OBJS1 = $(OBJS) pacc1.o

pacc1: $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $^

pacc1.o: pacc.c pacc1.c
	cp pacc1.c pacc-part.c
	$(CC) $(CFLAGS) -c -o $@ pacc.c

pacc1.c: pacc0
	./pacc0 > $@

OBJS2 = $(OBJS) pacc2.o

pacc2: $(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $^

pacc2.o: pacc.c pacc2.c
	cp pacc2.c pacc-part.c
	$(CC) $(CFLAGS) -c -o $@ pacc.c

pacc2.c: pacc1 pacc.pacc
	IFS= ./pacc1 `cat pacc.pacc` > $@

pacc: pacc2
	mv pacc2 pacc

clean:
	rm -f pacc1.c pacc2.c
	rm -f $(OBJS) pacc0.o pacc1.o pacc2.o
	rm -f pacc0 pacc1 pacc2 pacc
