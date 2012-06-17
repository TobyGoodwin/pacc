#! /bin/sh

# This code does not handle spacy filenames, nor should it.

nopacc() {
    loc=$1 err=$2
    echo $pacc parse.pacc
    r=$($pacc parse.pacc 2>&1 && echo 'parsed bad grammar by mistake!')
    check "$r" "parse.pacc:$loc: $err"
}
export -f nopacc

nopacc1() {
    err=$1
    echo $pacc parse.pacc
    r=$($pacc parse.pacc 2>&1 && echo 'parsed bad grammar by mistake!')
    check "$r" "$err"
}
export -f nopacc1

nocc() {
    loc=$1 err=$2
    echo $pacc parse.pacc
    $pacc parse.pacc || fail 'pacc failed on nocc test'
    echo cc parse.c
    cc parse.c
}
export -f nocc

target=$1
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
