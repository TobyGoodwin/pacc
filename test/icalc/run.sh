make -C icalc clean
if make -C icalc; then
    for in in icalc/*.in; do
	out=$(echo $in | sed 's/.in/.out/')
	# < $in ./icalc/icalc > $out 2>&1
	if < $in ./icalc/icalc 2>&1 | cmp -s $out; then
	    pass $in
	else
	    echo got:
		< $in ./icalc/icalc
	    echo expected:
		cat $out
	    fail $in
	fi
    done
else
    fail "cannot make icalc"
fi
