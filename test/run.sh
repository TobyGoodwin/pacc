#! /bin/sh

export passes=`mktemp`
export fails=`mktemp`

fail() {
    echo FAIL: "$1"
    echo -n . >> $fails
}
export -f fail

pass() {
    echo PASS: "$1"
    echo -n . >> $passes
}
export -f pass

check() {
    if [ "$1" = "$2" ]; then
	pass "$target: $1"
    else
	fail "$target: $1 (expected $2)"
    fi
}
export -f check

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

ts=${*:-bad/*.pacc emit/mk-*.c pacc/*.pacc java/java.pacc}

for t in $ts; do
    export target=$t
    export pacc=../pacc2
    runner=`echo $target | sed 's,/.*,/run.sh,'`
    $runner $target
done

echo
echo `stat '--printf=%s' $passes` passes
echo `stat '--printf=%s' $fails` fails
