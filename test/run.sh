#! /bin/sh

export passes=`mktemp`
export fails=`mktemp`
export expfails=`mktemp`

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

XXX() {
    echo expect fail
    echo -n . >> $expfails
}
export -f XXX

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
    sed -n '/\*\//q;2,$p' $1 > $t
    cat $t
    if test -s $t; then
	. $t
    else
	rm $t
	fail 'no script found'
    fi
    rm $t
}
export -f script_from

ts=${*:-bad/*.pacc emit/mk-*.c feed/*.pacc pacc/*.pacc}

for t in $ts; do
    export target=$t
    runner=`echo $target | sed 's,/.*,/run.sh,'`
    $runner $target
done

count() {
    stat '--printf=%s' $1
}

echo
echo $(count $passes) passes
echo $(count $fails) fails,  expected $(count $expfails)
rm $passes $fails $expfails
make -C.. test/clean
