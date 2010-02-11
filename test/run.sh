#! /bin/sh -e

build() {
    # Oh. Seems that commandline -e doesn't apply to functions.
    set -e
    echo build $1
    cp $1.pacc parse.pacc
    rm -f parse.c harness harness.o parse.o
    make harness
}

passes=0
fails=0

check() {
    r=`./harness "$1" 2>&1`
    if [ "$r" = "$2" ]; then
	echo "PASS: $1 ==> $2"
	passes=`expr $passes + 1`
    else
	echo "FAIL: $1 ==> $r (expected $2)"
	fails=`expr $fails + 1`
    fi
}

parse() {
    check "$1" "parsed with value $2"
}

noparse() {
    check "$1" "expected $2 at column $3"
}

cp parse.h-int parse.h

build val0
parse 5 5
noparse x A 0

build val1
parse 5 5
parse 6 6
noparse x A 0

build val2
parse 5 5
parse 6 6
noparse x P 0

build val3
parse 5.5 25
parse 5.6 30
parse 6.5 30
parse 6.6 36
noparse 5.x A 2

build val4
parse 5.5 25
parse 5.6 30
parse 6.5 30
parse 6.6 36
noparse 5.x A 2

build val5
parse .5 5
parse .6 6
noparse .x A 1

build val6
parse 5.5 25
parse 5.6 30
parse 6.5 30
parse 6.6 36
noparse x P 0
noparse 5.x A 2
noparse 6.x A 2
parse 5+5 10
parse 5+6 11
parse 6+5 11
parse 6+6 12
noparse 5+x A 2
noparse 6+x A 2
noparse 5.+ A 2

cp parse.h-chars parse.h

build lit0
parse foo foo
noparse bar A 0

build not0
parse a a
parse ac a
noparse ab A 1

build not1
noparse f S 0
noparse fo S 0
parse foo yes
noparse food S 3

build nest0
parse abcd yes

build nest1
parse abcd yes

build nest2
parse ab ab
parse cd cd
noparse acd '"b"' 1

build nest3
noparse ab '"d"' 2
noparse cd P 0
parse abd abd
parse acd acd

build rep0
parse xx xx
parse xyx xyx
parse xyyx xyyx
parse xyyyx xyyyx
parse xyyyyyyyyyyx xyyyyyyyyyyx

build rep1
parse xx ''
parse xyx y
parse xyyx yy

build rep2
noparse xx Y 1
parse xyx xyx
parse xyyx xyyx
parse xyyyx xyyyx
parse xyyyyyyyyyyx xyyyyyyyyyyx

build rep3
parse xx xx
parse xyx xyx
noparse xyyx '"x"' 2
noparse xyyyx '"x"' 2
noparse xyyyyyyyyyyx '"x"' 2

# This test needs to do something more clever with the results,
# otherwise we're not really proving that we're doing nested reps.
build rep4
parse a a
parse 'a b' 'a b'
parse 'a  b' 'a  b'

cp parse.h-int parse.h

build guard0
check 5 'hello, world!
parsed with value 5'
noparse x A 0

build guard1
parse 5 5
noparse 6 A 1

build guard2
parse 5 5
noparse 6 A 1

build guard3
parse 55 5
parse 66 6
noparse 56 A 2
noparse 65 A 2
noparse xy P 0

cp parse.h-chars parse.h

build guard4
parse a a
parse q q
parse z z
noparse A A 1
noparse '%' A 1

cp parse.h-int parse.h

build guard5
parse a9 9
parse z3 3
noparse zx A 2

cp parse.h-chars parse.h

build guard6
parse q q
parse qux qux
parse q39a q39a
parse q. q
parse q3. q3
noparse 37 Char 1

build guard7
parse '{}' '{}'
parse '{ any chars here }' '{ any chars here }'
noparse '{ no' '"}"' 4

build type0
parse 5 five

build type1
parse 5 5

build type2
parse a a
parse aa aa
parse aaaaa aaaaa
parse aabaa aa

cp parse.h-int parse.h

build type3
parse 78 78

cp parse.h-chars parse.h

build match0
parse 567 56

build match1
parse 567 7

build scope0
parse ab a
noparse ac B 1

# Currently, scope1 won't even compile
#build scope1
#parse ab a
#parse ac a

cp parse.h-int parse.h

baf_tests1() {
    parse 5 5
    parse 2+3 5
    parse '2*3' 6
    parse '2+3+4' 9
    parse '2*3+4' 10
    parse '2+3*4' 14
    parse '2*3*4' 24
}

build baf0
baf_tests1
noparse x Additive 0

build baf1
baf_tests1
noparse x Additive 0

baf_tests2() {
    baf_tests1
    parse '53*(13+75)' 4664
}

build baf2
baf_tests2
noparse x Additive 0

baf_tests3() {
    baf_tests2
    parse ' 456 + 123 * ( 543 + 7 * (987 + 2) + 6) ' 919512
}

build baf3
baf_tests3
noparse x Sum 0

cp parse.h-chars parse.h

build regress0
parse 'fred' 'fred'
parse 'fred ' 'fred'
noparse 'fred barney' End 5

cp parse.h-int parse.h
build regress1
parse 5 5

echo
echo $passes passes
echo $fails fails
if [ $fails -ne 0 ]; then exit 1; else exit 0; fi
