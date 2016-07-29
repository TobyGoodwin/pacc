#! /bin/sh

# This code does not handle spacy filenames, nor should it.

nopacc() {
    loc=$1 err=$2
    echo $pacc parse.pacc
    r=$($pacc parse.pacc 2>&1 && echo 'parsed bad grammar by mistake!')
    check "$r" "parse.pacc:$loc: $err"
}

nopacc1() {
    err=$1
    echo $pacc parse.pacc
    r=$($pacc parse.pacc 2>&1 && echo 'parsed bad grammar by mistake!')
    check "$r" "$err"
}

nofeed() {
    err=$1
    echo $pacc -f/dev/null parse.pacc
    r=$($pacc -f/dev/null parse.pacc 2>&1 &&
	    echo 'parsed bad grammar by mistake!')
    check "$r" "$err"
}

# We don't control the output of the C compiler, so can't be too picky about
# what it says. nocc tests are gcc specific, but hopefully not gcc-version
# specific.
contains() {
    echo "$1" | grep -q -F -e "$2"
}

nocc() {
    line=$1
    col=$2
    err=$3
    cflags=$4
    echo $pacc parse.pacc
    $pacc parse.pacc || fail 'pacc failed on nocc test'
    echo c99 $cflags parse.c
    r=$(c99 $cflags parse.c 2>&1 && echo 'compiled bad grammar by mistake!')
    if contains "$r" "parse.pacc:$line:$col: $err" ||
	    contains "$r" "parse.pacc:$line: $err"; then
	pass $target
    else
	fail "$target: did not find [parse.pacc:$line:$col: $err] in"
    fi
    echo "$r"
}

noccwarn() {
    nocc "$1" "$2" "$3" '-Werror'
}

case $target in
bad/empty0.pacc)
    script=bad/empty0.script ;;
*) 
    script=$target ;;
esac

pacc=../pacc
echo cp $target parse.pacc
cp $target parse.pacc
script_from $script
