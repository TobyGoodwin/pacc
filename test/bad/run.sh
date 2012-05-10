#! /bin/sh

# This code does not handle spacy filenames, nor should it.

# A problem identified by this code will normally result in *2*
# failures: one because the expected error message is not output, and a
# second because pacc itself succeeds. I do not consider this a bug.
nopacc() {
    loc=$1 err=$2
    echo $pacc $target
    r=`$pacc $target 2>&1 && fail 'parsed bad grammar by mistake!'`
    check "$r" "$target:$loc: $err"
}
export -f nopacc

nocc() {
    loc=$1 err=$2
    echo $pacc -o parse.c $target
    $pacc -o parse.c $target || fail 'pacc failed on nocc test'
    echo cc parse.c
    cc parse.c
}
export -f nocc

target=$1
script_from $target
