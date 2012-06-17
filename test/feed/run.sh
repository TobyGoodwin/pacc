#! /bin/sh

# This code does not handle spacy filenames, nor should it.

parse() {
    exp="$1"
    shift
    echo ./feeder "$@"
    r=`./feeder "$@" 2>&1`
    check "$r" "parsed with value $exp"
}

parse_exact() {
    echo ./feeder "$1"
    r=`./feeder "$1" 2>&1`
    check "$r" "$2"
}

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

type() {
    case $1 in
	chars|int) echo cp parse.h-$1 parse.h; cp parse.h-$1 parse.h ;;
	*) fail "bad type $1" ;;
    esac
    make -C.. test/feeder || exit
}

make -C.. test/clean
echo cp $target parsefeed.pacc
cp $target parsefeed.pacc
script_from $target
