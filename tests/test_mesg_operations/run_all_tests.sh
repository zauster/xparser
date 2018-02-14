#!/bin/bash

export TESTS="sorting filtering randomizing filtering_and_random"
#export TESTS="randomizing"
#export TESTS="sorting"
#export TESTS="filtering"
#export TESTS="filtering_and_random"

export BASE=$PWD

for i in $TESTS; do
	
	# Cleanup files
	rm $BASE//models/$i/test_output/test_output.txt

	echo "Running model_$i"

	cd $BASE/models/$i
		
	echo "Test $i: " >>"$BASE//models/$i/test_output/test_output.txt"

	./main 4 0.xml 2>>"$BASE//models/$i/test_output/test_output.txt"

	echo "" >> "$BASE//models/$i/test_output/test_output.txt"
	
	echo "------------------------" >> "$BASE//models/$i/test_output/test_output.txt"
	echo "------------------------"
done