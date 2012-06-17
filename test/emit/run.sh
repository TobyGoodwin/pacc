#! /bin/sh

# This code does not handle spacy filenames, nor should it.

parse() {
    echo ./harness "$1"
    r=`./harness "$1" 2>&1`
    check "$r" "parsed with value $2"
}
export -f parse

parse_exact() {
    echo ./harness "$1"
    r=`./harness "$1" 2>&1`
    check "$r" "$2"
}
export -f parse_exact

noparse() {
    echo ./harness "$1"
    r=`./harness "$1" 2>&1`
    check "$r" "arg:1:$3: expected $2"
}
export -f noparse

type() {
    case $1 in
	chars|int) echo cp parse.h-$1 parse.h; cp parse.h-$1 parse.h ;;
	*) fail "bad type $1" ;;
    esac
    rm -f emitter emitter.o parse.c parse.o harness harness.o
    make -C.. test/emitter
    echo ./emitter -o parse.c parse.pacc
    ./emitter -o parse.c parse.pacc
    make -C.. test/harness
}
export -f type

# For hysterical raisins...
chars() {
    type chars
}
export -f chars

int() {
    type int
}
export -f int

target=$1
echo x > parse.pacc
echo cp $target emitter.c
cp $target emitter.c
script_from $target
