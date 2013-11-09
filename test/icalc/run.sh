make -C icalc clean
if make -C icalc; then
    for in in icalc/*.in; do
	out=$(echo $in | sed 's/.in/.out/')
	# < $in ./icalc/icalc > $out 2>&1
	if < $in ./icalc/icalc 2>&1 | cmp -s $out; then
	    pass $in
	else
	    fail $out
	fi
    done
else
    fail "cannot make icalc"
fi
