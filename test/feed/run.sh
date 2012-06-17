#! /bin/sh

# This code does not handle spacy filenames, nor should it.

parse() {
    exp="$1"
    shift
    echo ./feeder "$@"
    r=`./feeder "$@" 2>&1`
    check "$r" "parsed with value $exp"
}
export -f parse

parse_exact() {
    echo ./feeder "$1"
    r=`./feeder "$1" 2>&1`
    check "$r" "$2"
}
export -f parse

end() {
    echo ./feeder "$@"
    r=`./feeder "$@" 2>&1`
    check "$r" "unexpected end of input"
}

noparse() {
    coord="$1"
    shift
    exp="$1"
    shift
    echo ./feeder "$@"
    r=`./feeder "$@" 2>&1`
    check "$r" "arg:$coord: expected $exp"
}
export -f noparse

type() {
    case $1 in
	chars|int) echo cp parse.h-$1 parse.h; cp parse.h-$1 parse.h ;;
	*) fail "bad type $1" ;;
    esac
    make -C.. test/clean
    make -C.. test/feeder || exit
}
export -f type

target=$1
echo cp $target parsefeed.pacc
cp $target parsefeed.pacc
script_from $target
