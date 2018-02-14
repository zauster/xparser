#!/bin/bash

#export TESTS=$(seq 1 1 4)
export TESTS=$(seq 1 1 16)

export SUBTESTS="a b c d"

export BASE=$PWD

# Cleanup files
rm output/test_output.txt

for i in $TESTS; do
	echo "Running models/test_$i"

	for j in $SUBTESTS; do

	echo "$j"
		
		cd $BASE/models/test_$i/$j
		
		echo -n "Test $i/$j: " >>"$BASE/output/test_output.txt"

		./main 1 0.xml 2>>"$BASE/output/test_output.txt"

		echo "" >> "$BASE/output/test_output.txt"
	done
	
	echo "------------------------" >> "$BASE/output/test_output.txt"
	echo "------------------------"
done