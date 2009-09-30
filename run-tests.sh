#! /bin/sh -e

build() {
    # Oh. Seems that commandline -e doesn't apply to functions.
    set -e
    echo build $1.c
    cp test/$1.c mktree.c
    # The rm line below distresses me. As far as I can see, the Makefile
    # describes a chain of dependencies from foo to mktree.c. So why
    # does make need this helping hand? My best guess is that to have a
    # .c depending on a binary interferes with implicit rules.
    rm -f mktree.o pacc foo gen.c foo.o
    make foo
}

check() {
    r=`./foo "$1" | sed -n '/\(not \|\)parsed\(\| with value \)\(.*\)/s//\3/p'`
    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
    else
	echo "FAIL: $1 ==> $r (expected $2)" 1>&2
	exit 1
    fi
}

build mk-val0
check 5 5
check x ''

build mk-val1
check 5 5
check 6 6
check x ''

build mk-val2
check 5 5
check 6 6
check x ''

build mk-val3
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

build mk-val4
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

build mk-val5
check .5 5
check .6 6
check .x ''

build mk-val6
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check x ''
check 5.x ''
check 6.x ''
check 5+5 10
check 5+6 11
check 6+5 11
check 6+6 12
check 5+x ''
check 6+x ''
check 5.+ ''

build mk-lit0
check foo foo
check bar ''

build mk-and0
check ab a
check ac ''

build mk-not0
check a a
check ac a
check ab ''

build mk-not1
check f ''
check fo ''
check foo yes
check food ''

build mk-rep0
check xx xx
check xyx xyx
check xyyx xyyx
check xyyyx xyyyx
check xyyyyyyyyyyx xyyyyyyyyyyx

build mk-rep1
check xx ''
check xyx xyx
check xyyx xyyx
check xyyyx xyyyx
check xyyyyyyyyyyx xyyyyyyyyyyx

build mk-rep2
check xx xx
check xyx xyx
check xyyx ''
check xyyyx ''
check xyyyyyyyyyyx ''

# This test needs to do something more clever with the results,
# otherwise we're not really proving that we're doing nested reps.
build mk-rep3
check a a
check 'a b' 'a b'
check 'a  b' 'a  b'

build mk-guard0
check 5 5
check x ''

build mk-guard1
check 5 5
check 6 ''

build mk-guard2
check 55 5
check 66 6
check 56 ''
check 65 ''
check xy ''

build mk-guard3
check a a
check q q
check z z
check A ''
check '%' ''

build mk-guard4
check a9 9
check z3 3
check zx ''

build mk-guard5
check q q
check qux qux
check q39a q39a
check q. q
check q3. q3
check 37 ''

build mk-guard6
check '{}' '{}'
check '{ any chars here }' '{ any chars here }'
check '{ no' ''

build mk-type0
check 5 five

build mk-type1
check 5 5

build mk-type2
check a a
check aa aa
check aaaaa aaaaa
check aabaa aa

build mk-match0
check 567 56

build mk-match1
check 567 7

baf_tests1() {
    check 5 5
    check x ''
    check 2+3 5
    check '2*3' 6
    check '2+3+4' 9
    check '2*3+4' 10
    check '2+3*4' 14
    check '2*3*4' 24
}

build mk-baf0
baf_tests1

build mk-baf1
baf_tests1

baf_tests2() {
    baf_tests1
    check '53*(13+75)' 4664
}

build mk-baf2
baf_tests2

baf_tests3() {
    baf_tests2
    check ' 456 + 123 * ( 543 + 7 * (987 + 2) + 6) ' 919512
}

build mk-baf3
baf_tests3

build mk-regress0
check 'fred' 'fred'
