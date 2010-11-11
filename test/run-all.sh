#! /bin/sh

passes=0
fails=0

> /tmp/p
> /tmp/f

fail() {
    echo FAIL: "$1"
    echo -n . >> /tmp/f
}
export -f fail

pass() {
    echo PASS: "$1"
    echo -n . >> /tmp/p
}
export -f pass

check() {
    # Like make, say what we are about to do, in case we want to do it
    # manually in a momemnt...
    echo ./harness "$1"
    # ...then do it, captuing all output.
    r=`./harness "$1" 2>&1`

    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
	passes=`expr $passes + 1`
    else
	echo "FAIL $target: $1 ==> $r (expected $2)"
	fails=`expr $fails + 1`
    fi
}

check2() {
    if [ "$1" = "$2" ]; then
	pass "$target: $1"
    else
	fail "$target: $1 (expected $2)"
    fi
}
export -f check2

script_from() {
    # Extract a test script from a C-style comment in a test case. The
    # sed script is surprisingly hard to get right. It's really hard to
    # match a line (like the comment lines) without also printing it.
    # Also, I was surprised that '{d;q}' means exactly the same as 'd'.

    t=`mktemp`
    sed -n '/\*\//q;2,$p' $target > $t
    if test -s $t; then
	. $t
    else
	rm $t
	fail 'no script found'
    fi
    rm $t
}
export -f script_from

parse() {
    check "$1" "parsed with value $2"
}

parse_file() {
    # It would be nicer to have a different harness that can read a
    # file. One problem with doing it here is that even with the "``"
    # syntax, Bourne *still* messes with the value, stripping trailing
    # newlines, whatever IFS is set to.
    check "`cat $1`" "parsed with value $2"
}

noparse() {
    check "$1" "arg:1:$3: expected $2"
}

noparse_file() {
    check "`cat $1`" "expected $2 at column $3"
}

run() {
    target=$1
#    echo $target
#    type=`sed -n '/^type: /s///p' $target`
#    # An historical default
#    case $type in '') type=`sed -n 2p $target` ;; esac
#    case $type in
#	chars|int) cp parse.h-$type parse.h ;;
#	*)
#	    #echo "FAIL $target: bad type $type"
#	    #fails=`expr $fails + 1`
#	    #return 1
#	    ;;
#    esac
#    src=`sed -n '/^source: /s///p' $target`
#    # Another historical default
#    case $src in '') src=arg ;; esac
#    case $src in
#	arg|file) cp harness-$src.c harness.c ;;
#	*)
#	    echo "FAIL $target: bad source $src"
#	    fails=`expr $fails + 1`
#	    return 1
#	    ;;
#    esac
    # Need better Makefiles?
#    rm -f emitter emitter.o parse.c parse.o harness harness.o
    case $target in
	emit/*)
	    export pacc=../pacc2
	    emit/run.sh $target
	    return

	    echo x > parse.pacc # Makefile has parse.c depends on parse.pacc
	    cp $target emitter.c
	    make emitter
	    echo "./emitter -o parse.c parse.pacc"
	    ./emitter -o parse.c parse.pacc

	    ;;
	bad/*)
	    export pacc=../pacc2
	    bad/run.sh $target
	    return
	    ;;
	pacc/*)
	    export pacc=../pacc2
	    pacc/run.sh $target
	    return
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

ts=${*:-bad/*.pacc emit/mk-*.c pacc/*.pacc java/java.pacc}

for t in $ts; do
    run $t
done

echo
echo `stat '--printf=%s' /tmp/p` passes
echo `stat '--printf=%s' /tmp/f` fails
