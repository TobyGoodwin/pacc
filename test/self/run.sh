#! /bin/sh

# This code does not handle spacy filenames, nor should it.

parse_exact() {
    echo ./solo "$1"
    r=`./solo "$1" 2>&1`
    check "$r" "$2"
}

parse() {
    parse_exact "$1" "parsed with value $2"
}

noparse_exact() {
    echo ./solo "$1"
    r=`./solo "$1" 2>&1`
    check "$r" "$2"
}

noparse() {
    noparse_exact "$1" "arg:1:$3: expected $2"
}

echo cp $target solo.pacc
cp $target solo.pacc
make -C .. test/solo
. $(echo $target | sed 's/.pacc/.test/')
