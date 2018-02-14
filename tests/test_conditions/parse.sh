#!/bin/bash

# Path to xparser
export FLAME_XPARSER_DIR="$PWD/../.."

cd $FLAME_XPARSER_DIR

echo "Now here: $PWD"

export TESTS=$(seq 1 1 16)
#export TESTS=$(seq 1 1 1)

for i in $TESTS; do

	# Parse model a (time condition A false, memory condition B false)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/a"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model.xml

	cd $MODEL_DIR

	make clean all

	# Parse model b (time condition A false, memory condition B true)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/b"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model.xml

	cd $MODEL_DIR

	make clean all

	# Parse model c (time condition A true, memory condition B false)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/c"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model.xml

	cd $MODEL_DIR

	make clean all

	# Parse model d (time condition A true, memory condition B true)
	export MODEL_DIR="$FLAME_XPARSER_DIR/tests/test_conditions/models/test_$i/d"

	cd $FLAME_XPARSER_DIR

	./xparser $MODEL_DIR/model.xml

	cd $MODEL_DIR

	make clean all
done

echo 'Script done.'
