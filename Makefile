CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS = -g

all: pacc

OBJS = arg.o cook.o emit.o error.o load.o main.o preen.o sugar.o syntax.o template.o utf8.o

pacc0: $(OBJS) pacc0.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc0.o: pacc0.c pacc.h syntax.h

pacc1: $(OBJS) pacc1.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc1.c: pacc0
	./pacc0 /dev/null -o $@

pacc2: $(OBJS) pacc2.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc2.c: pacc1 pacc.pacc
	./pacc1 pacc.pacc -o $@

pacc3: $(OBJS3) pacc3.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc3.c: pacc2 pacc.pacc
	./pacc2 pacc.pacc -o $@

pacc: pacc2 sane
	cp pacc2 pacc

template.c: pacc.tmpl template.sh
	sh template.sh > $@

arg.o: arg.h

cook.o: cook.h arg.h syntax.h

emit.o: emit.h error.h syntax.h template.h

error.o: error.h

load.o: load.h

preen.o: preen.h syntax.h

sugar.o: sugar.h syntax.h

syntax.o: syntax.h

template.o: template.h

utf8.o: utf8.h

%.d: %.c
	sed '/^#/d' $^ > $@

.PHONY: sane check clean
sane: pacc2.d pacc3.d
	diff $^

check:
	cd test && sh run.sh

clean:
	rm -f pacc0 pacc1 pacc2 pacc3 pacc
	rm -f $(OBJS)
	rm -f pacc0.o pacc1.o pacc2.o pacc3.o
	rm -f pacc1.c pacc2.c pacc3.c pacc2.d pacc3.d template.c
