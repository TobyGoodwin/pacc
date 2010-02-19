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

run() {
    target="$1"
    echo $target
    type=`sed -n 2p $target`
    case $type in
	chars|int) cp parse.h-$type parse.h ;;
	*)
	    echo "FAIL $target: bad type $type"
	    fails=`expr $fails + 1`
	    return 1
	    ;;
    esac
    # Need better Makefiles?
    rm -f emitter emitter.o parse.c parse.o harness harness.o
    case $target in
	emit/*)
	    > parse.pacc # Makefile has parse.c depends on parse.pacc
	    cp $target emitter.c
	    make emitter
	    ./emitter -o parse.c Makefile
	    ;;
	*)
	    cp $target parse.pacc
	    ;;
    esac
    make harness
    t=`mktemp`
    egrep '^check|^parse|^noparse' $target > $t
    if test -s $t; then
	# Turn off globbing for the duration, so 2*3 works as expected.
	set -f
	. $t
	set +f
	rm $t
    else
	echo "FAIL $target: no tests found"
	fails=`expr $fails + 1`
    fi
}

ts=${*:-emit/mk-*.c pacc/*.pacc}

for t in $ts; do
    run $t
done

echo
echo $passes passes
echo $fails fails
[ $fails -eq 0 ]
