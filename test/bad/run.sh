#! /bin/sh

# This code does not handle spacy filenames, nor should it.

# We don't control the output of the C compiler, so can't be too picky about
# what it says. nocc tests are gcc specific, but hopefully not gcc-version
# specific.
acheck() {
    if echo "$1" | grep -q -F -e "$2"; then
	pass "$target: $1"
    else
	fail "$target: $1 (expected match on $2)"
    fi
}

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

nocc() {
    err=$1
    cflags=$2
    echo $pacc parse.pacc
    $pacc parse.pacc || fail 'pacc failed on nocc test'
    echo c99 parse.c
    r=$(c99 $cflags parse.c 2>&1 && echo 'compiled bad grammar by mistake!')
    acheck "$r" "$err"
}

noccwarn() {
    nocc "$1" '-Werror'
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
