#! /bin/sh

passes=$(mktemp)
fails=$(mktemp)
expfails=$(mktemp)

fail() {
    echo FAIL: "$1"
    echo -n . >> $fails
}

pass() {
    echo PASS: "$1"
    echo -n . >> $passes
}

XXX() {
    echo expect fail
    echo -n . >> $expfails
}

check() {
    if [ "$1" = "$2" ]; then
	pass "$target: $1"
    else
	fail "$target: $1 (expected $2)"
    fi
}

script_from() {
    # Extract a test script from a C-style comment in a test case. The
    # sed script is surprisingly hard to get right. It's really hard to
    # match a line (like the comment lines) without also printing it.
    # Also, I was surprised that '{d;q}' means exactly the same as 'd'.

    t=$(mktemp)
    sed -n '/\*\//q;2,$p' $1 > $t
    if test -s $t; then
	. $t
    else
	rm $t
	fail 'no script found'
    fi
    rm $t
}

ts=${*:-bad/*.pacc emit/mk-*.c feed/*.pacc icalc/ pacc/*.pacc self/*.pacc}

for t in $ts; do
    target=$t
    runner=$(echo $target | sed 's,/.*,/run.sh,')
    . $runner
done

count() {
    stat '--printf=%s' $1
}

echo
echo $(count $passes) passes
echo $(count $fails) fails,  expected $(count $expfails)
rm $passes $fails $expfails
#make -C.. test/clean
