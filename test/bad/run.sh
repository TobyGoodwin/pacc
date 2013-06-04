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

nocc() {
    err=$1
    cflags=$2
    echo $pacc parse.pacc
    $pacc parse.pacc || fail 'pacc failed on nocc test'
    echo c99 parse.c
    r=$(c99 $cflags parse.c 2>&1 && echo 'compiled bad grammar by mistake!')
    check "$r" "$err"
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
