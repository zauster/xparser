#!/bin/bash

#export TESTS=$(seq 1 1 16)
export TESTS=$(seq 1 1 4)

export BASE="$PWD/../tests"


# Cleanup files
rm "$BASE/../output/flame_output.txt"
rm "$BASE/../output/test_output.txt"


for i in $TESTS; do

	cd $BASE/test_$i
	
	echo -n "Test $i: " >>"$BASE/../output/test_output.txt"

	./main 1 0.xml >>"$BASE/../output/flame_output.txt" 2>>"$BASE/../output/test_output.txt"

	echo "">>"$BASE/../output/test_output.txt"
done