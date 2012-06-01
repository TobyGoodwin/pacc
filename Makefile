CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS = -g

all: pacc sane

OBJS = arg.o cook.o emit.o error.o load.o main.o preen.o sugar.o syntax.o template.o utf8.o

OBJS0 = $(OBJS) pacc0.o

pacc0: $(OBJS0) 
	$(CC) $(LDFLAGS) -o $@ $^

pacc0.o: pacc0.c pacc.h syntax.h

OBJS1 = $(OBJS) pacc1.o

pacc1: $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $^

pacc1.c: pacc0 syntax.h
	./pacc0 pacc.pacc -o $@

OBJS2 = $(OBJS) pacc2.o

pacc2: $(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $^

pacc2.c: pacc1 pacc.pacc syntax.h
	./pacc1 pacc.pacc -o $@

OBJS3 = $(OBJS) pacc3.o

pacc3: $(OBJS3)
	$(CC) $(LDFLAGS) -o $@ $^

pacc3.c: pacc2 pacc.pacc syntax.h
	./pacc2 pacc.pacc -o $@

pacc: pacc2
	cp pacc2 pacc

template.c: pacc.tmpl template.sh
	sh template.sh > $@

arg.o: arg.c arg.h

cook.o: cook.c cook.h arg.h syntax.h

emit.o: emit.c emit.h error.h syntax.h template.h

error.o: error.c error.h

load.o: load.c load.h

preen.o: preen.c preen.h syntax.h

sugar.o: sugar.c sugar.h syntax.h

syntax.o: syntax.c syntax.h

template.o: template.h

utf8.o: utf8.h

%.d: %.c
	sed '/^#/d' $^ > $@

sane: pacc2.d pacc3.d
	diff $^

check:
	cd test; $(MAKE) check

clean:
	rm -f pacc0 pacc1 pacc2 pacc3 pacc
	rm -f $(OBJS)
	rm -f pacc0.o pacc1.o pacc2.o pacc3.o
	rm -f pacc1.c pacc2.c pacc3.c template.c
