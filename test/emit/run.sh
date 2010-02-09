#! /bin/sh

passes=0
fails=0

check() {
    r=`./harness "$1" 2>&1`
    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
	passes=`expr $passes + 1`
    else
	echo "FAIL $target: $1 ==> $r (expected $2)"
	fails=`expr $fails + 1`
    fi
}

parse() {
    check "$1" "parsed with value $2"
}

noparse() {
    check "$1" "expected $2 at column $3"
}

for target in mk-*.c; do
#for target in mk-bind*.c; do
    echo $target
    cp $target emitter.c
    cp ../parse.h-`sed -n 2p $target` parse.h
    rm -f emitter emitter.o parse.c parse.o harness harness.o
    make harness
    # If we use a pipe here, the entire while loop runs in a subshell
    # and $passes and $fails don't get set.
    t=`mktemp`
    egrep '^check|^parse|^noparse' $target > $t
    # Turn off globbing for the duration, so 2*3 works as expected.
    set -f
    . $t
    set +f
    rm $t
done

echo
echo $passes passes
echo $fails fails
if [ $fails -ne 0 ]; then exit 1; else exit 0; fi

