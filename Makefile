CC = c99
CFLAGS = -g -Wall -Wextra -I.
LDFLAGS = -g

.PHONY: all
all: pacc

OBJS = arg.o cook.o emit.o error.o load.o main.o preen.o sugar.o syntax.o template.o utf8.o

pacc0: $(OBJS) pacc0.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc0.o: pacc0.c syntax.h

pacc1: $(OBJS) pacc1.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc1.c: pacc0
	./pacc0 -n pacc /dev/null -o $@

pacc2: $(OBJS) pacc2.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc2.c: pacc1 pacc.pacc
	./pacc1 pacc.pacc -o $@

pacc3: $(OBJS) pacc3.o
	$(CC) $(LDFLAGS) -o $@ $^

pacc3.c: pacc2 pacc.pacc
	./pacc2 pacc.pacc -o $@

pacc: pacc2.d pacc3.d
	diff -u $^
	cp pacc2 $@

template.c: pacc.tmpl template.sh
	sh template.sh > $@

arg.o: arg.h error.h

cook.o: cook.h arg.h error.h syntax.h

emit.o: emit.h error.h syntax.h template.h

error.o: error.h

load.o: load.h error.h

preen.o: preen.h error.h syntax.h

sugar.o: sugar.h error.h syntax.h

syntax.o: error.h syntax.h utf8.h

template.o: template.h

utf8.o: utf8.h

%.d: %.c
	sed '/^#line/d' $^ > $@

boot: boot.o error.o load.o pacc2.o syntax.o utf8.o
	$(CC) $(LDFLAGS) -o $@ $^

# Testing

.PHONY: check
check: pacc
	cd test && sh run.sh

.PHONY: clean
clean: test/clean
	@echo clean
	@rm -f pacc0 pacc1 pacc2 pacc3 pacc
	@rm -f $(OBJS)
	@rm -f pacc0.o pacc1.o pacc2.o pacc3.o
	@rm -f pacc1.c pacc2.c pacc3.c pacc2.d pacc3.d template.c

test/harness: test/harness.o test/parse.o
	$(CC) $(CFLAGS) -o $@ $^

test/harness.o: test/parse.h

test/parse.c test/parse.h: test/parse.pacc
	./pacc $^

test/emitter: $(OBJS) test/emitter.o
	$(CC) $(LDFLAGS) -o $@ $^

test/feeder: test/feeder.o test/parsefeed.o test/partial.o
	$(CC) $(LDFLAGS) -o $@ $^

test/feeder.o: test/parsefeed.h

test/parsefeed.c test/parsefeed.h test/partial.c: test/parsefeed.pacc
	./pacc -d -ftest/partial.c $^

test/solo: test/solo.pacc
	./pacc test/solo.pacc
	$(CC) -o $@ test/solo.c

.PHONY: test/clean
test/clean: 
	@echo test/clean
	@rm -f test/harness test/harness.o test/parse.o
	@rm -f test/parse.c test/parse.h test/parse.pacc
	@rm -f test/emitter test/emitter.o test/emitter.c
	@rm -f test/feeder test/feeder.o test/parsefeed.o test/partial.o
	@rm -f test/parsefeed.c test/parsefeed.h test/partial.c
	@rm -f test/parsefeed.pacc
	@rm -f test/solo.pacc test/solo.c test/solo
	@rm -f test/icalc/icalc test/icalc/main.o
	@rm -f test/icalc/parse1.o test/icalc/parse2.o
	@rm -f test/icalc/parse1.c test/icalc/parse2.c test/icalc/parse.h

# Maintainer targets
.PHONY: release
release: doc
	mkdir $$(cat version)
	cp -a --parents $$(cat MANIFEST) $$(cat version)
	tar cfj $$(cat version).tar.bz2 $$(cat version)
	rm -r $$(cat version)

.PHONY: versions
versions:
	echo -n pacc- > version; git describe >> version
	date '+@set UPDATED %d %B %y' > doc/version.texi
	date '+@set UPDATED-MONTH %B %y' >> doc/version.texi
	echo -n '@set EDITION ' >> doc/version.texi
	git describe >> doc/version.texi
	echo -n '@set VERSION ' >> doc/version.texi
	git describe >> doc/version.texi

.PHONY: doc
doc: doc/pacc.info doc/pacc.man doc/pacc.pdf doc/pacc/index.html

DOC_SRC = doc/pacc.texi doc/fdl.texi

doc/pacc.info: $(DOC_SRC)
	cd doc; makeinfo pacc.texi

doc/pacc.man: $(DOC_SRC) pacc
	help2man ./pacc > doc/pacc.man

doc/pacc.pdf: $(DOC_SRC)
	cd doc; texi2pdf pacc.texi

doc/pacc/index.html: $(DOC_SRC)
	cd doc; makeinfo --html pacc.texi

.PHONY: doc doc/clean
doc/clean:
	@echo doc/clean
	@rm -f doc/pacc.info doc/pacc.man doc/pacc.pdf
	@rm -rf doc/pacc
