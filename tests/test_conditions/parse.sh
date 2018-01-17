#!/bin/bash

# Path to xparser
export FLAME_XPARSER_DIR="$PWD/../.."

cd $FLAME_XPARSER_DIR

echo "Now here: $PWD"

#export TESTS=$(seq 1 1 16)
export TESTS=$(seq 1 1 1)

for i in $TESTS; do

	# Parse model ab (time condition A false)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/ab"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model_ab.xml

	cd $MODEL_DIR

	make clean all

	# Parse model cd (time condition A true)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/cd"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model_cd.xml

	cd $MODEL_DIR

	make clean all

done

echo 'Script done.'
