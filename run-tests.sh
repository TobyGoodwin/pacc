#! /bin/sh -e

build() {
    echo build $1.c
    cp $1.c mk-target.c
    make foo
}

check() {
    r=`./foo $1 | sed -n '/\(not \|\)parsed\(\| with value \)\(.*\)/s//\3/p'`
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

build mk-val2
check 5 5
check 6 6
check x ''

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

build mk-type0
check 5 five

build mk-type1
check 5 5

build mk-type2
check a a
check aa aa
check aaaaa aaaaa
check aabaa aa

baf_tests() {
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
baf_tests

build mk-baf1
baf_tests
