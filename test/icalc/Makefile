CFLAGS = -std=c99
PACC = ../../pacc

icalc: parse1.o parse2.o main.o
	$(CC) -o $@ $^

main.o: main.c parse.h
parse1.o: parse1.c
parse2.o: parse2.c

parse.h parse1.c parse2.c: icalc.pacc
	$(PACC) -dparse.h -o parse1.c -f parse2.c $<

clean:
	rm -f icalc main.o parse1.o parse2.o parse.h parse1.c parse2.c
