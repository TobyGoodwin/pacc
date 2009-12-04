#! /bin/sh -e

build() {
    # Oh. Seems that commandline -e doesn't apply to functions.
    set -e
    echo build $1
    cp $1.pacc test.pacc
    rm -f parse.c harness harness.o parse.o
    make harness
}

check() {
    r=`./harness "$1" | sed -n '/\(not \|\)parsed\(\| with value \)\(.*\)/s//\3/p'`
    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
    else
	echo "FAIL: $1 ==> $r (expected $2)" 1>&2
	exit 1
    fi
}

cp parse.h-int parse.h

build val0
check 5 5
check x ''

build val1
check 5 5
check 6 6
check x ''

build val2
check 5 5
check 6 6
check x ''

build val3
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

build val4
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

build val5
check .5 5
check .6 6
check .x ''

build val6
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

cp parse.h-chars parse.h

build lit0
check foo foo
check bar ''

build and0
check ab a
check ac ''

build not0
check a a
check ac a
check ab ''

build not1
check f ''
check fo ''
check foo yes
check food ''

build rep0
check xx xx
check xyx xyx
check xyyx xyyx
check xyyyx xyyyx
check xyyyyyyyyyyx xyyyyyyyyyyx

build rep1
check xx ''
check xyx y
check xyyx yy

build rep2
check xx ''
check xyx xyx
check xyyx xyyx
check xyyyx xyyyx
check xyyyyyyyyyyx xyyyyyyyyyyx

build rep3
check xx xx
check xyx xyx
check xyyx ''
check xyyyx ''
check xyyyyyyyyyyx ''

# This test needs to do something more clever with the results,
# otherwise we're not really proving that we're doing nested reps.
build rep4
check a a
check 'a b' 'a b'
check 'a  b' 'a  b'

cp parse.h-int parse.h

build guard0
check 5 5
check x ''

build guard1
check 5 5
check 6 ''

build guard2
check 5 5
check 6 ''

build guard3
check 55 5
check 66 6
check 56 ''
check 65 ''
check xy ''

cp parse.h-chars parse.h

build guard4
check a a
check q q
check z z
check A ''
check '%' ''

cp parse.h-int parse.h

build guard5
check a9 9
check z3 3
check zx ''

cp parse.h-chars parse.h

build guard6
check q q
check qux qux
check q39a q39a
check q. q
check q3. q3
check 37 ''

build guard7
check '{}' '{}'
check '{ any chars here }' '{ any chars here }'
check '{ no' ''

build type0
check 5 five

build type1
check 5 5

build type2
check a a
check aa aa
check aaaaa aaaaa
check aabaa aa

cp parse.h-int parse.h

build type3
check 78 78

cp parse.h-chars parse.h

build match0
check 567 56

build match1
check 567 7

cp parse.h-int parse.h

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

build baf0
baf_tests1

build baf1
baf_tests1

baf_tests2() {
    baf_tests1
    check '53*(13+75)' 4664
}

build baf2
baf_tests2

baf_tests3() {
    baf_tests2
    check ' 456 + 123 * ( 543 + 7 * (987 + 2) + 6) ' 919512
}

build baf3
baf_tests3

cp parse.h-chars parse.h

build regress0
check 'fred' 'fred'
check 'fred ' 'fred'
check 'fred barney' ''

build regress1
check '5' ''
