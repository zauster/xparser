#!/bin/bash

# Path to xparser
export FLAME_XPARSER_DIR="$PWD/../.."

cd $FLAME_XPARSER_DIR

echo "Now here: $PWD"

export TESTS="sorting filtering randomizing filtering_and_random"
#export TESTS="randomizing"
#export TESTS="sorting"
#export TESTS="filtering"
#export TESTS="filtering_and_random"

for i in $TESTS; do

	# Parse model 
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_mesg_operations/models/$i"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model.xml

	cd $MODEL_DIR

	# Build model
	make clean all

done

echo 'Script done.'
