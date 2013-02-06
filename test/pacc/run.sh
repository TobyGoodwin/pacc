#! /bin/sh

# This code does not handle spacy filenames, nor should it.

parse_exact() {
    echo ./harness "$1"
    r=`./harness "$1" 2>&1`
    check "$r" "$2"
}

parse() {
    parse_exact "$1" "parsed with value $2"
}

noparse_exact() {
    echo ./harness "$1"
    r=`./harness "$1" 2>&1`
    check "$r" "$2"
}

noparse() {
    noparse_exact "$1" "arg:1:$3: expected $2"
}

type() {
    case $1 in
	chars|int|void) echo cp parse.h-$1 parse.h; cp parse.h-$1 parse.h ;;
	*) fail "bad type $1" ;;
    esac
    rm -f emitter emitter.o parse.c parse.o harness harness.o
    echo make -C .. test/harness
    make -C .. test/harness
}

# For hysterical raisins...
chars() {
    type chars
}

int() {
    type int
}

echo cp $target parse.pacc
cp $target parse.pacc
script_from $target
