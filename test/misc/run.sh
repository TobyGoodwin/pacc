# check that #line directives in pacc1.c and pacc2.c are correct
for f in ../pacc1.c ../pacc2.c; do
	awk '/^#line.*pacc[12]/ { if ($2 != NR + 1) { print "bad #line", $2, "at line", NR; exit 1 } }' $f
done || fail "bad line directive"
