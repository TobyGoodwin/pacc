#! /bin/sh

check() {
    r=`./foo $1 | sed -n '/\(not \|\)parsed\(\| with value \)\(.*\)/s//\3/p'`
    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
    else
	echo "FAIL: $1 ==> $r (expected $2)" 1>&2
	exit 1
    fi
}

cp mk-val0.c mk-target.c
make foo
check 5 5
check x ''

cp mk-val1.c mk-target.c
make foo
check 5 5
check 6 6
check x ''

cp mk-val2.c mk-target.c
make foo
check 5 5
check 6 6
check x ''

cp mk-val3.c mk-target.c
make foo
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

cp mk-val4.c mk-target.c
make foo
check 5.5 25
check 5.6 30
check 6.5 30
check 6.6 36
check 5.x ''

cp mk-val5.c mk-target.c
make foo
check .5 5
check .6 6
check .x ''

cp mk-val6.c mk-target.c
make foo
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

cp mk-val2.c mk-target.c
make foo
check 5 5
check 6 6
check x ''

cp mk-baf.c mk-target.c
make foo
check 5 5
check x ''
check 2+3 5
check '2*3' 6
check '2+3+4' 9
check '2*3+4' 10
check '2+3*4' 14
check '2*3*4' 24
