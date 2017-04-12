#!/bin/bash
# Script to parse and build model.xml

# set parser mode
export PARSE_DEBUG=1
export PARSE_PRODUCTION=0
export PARSE_SERIAL=1
export PARSE_PARALLEL=0

export BUILD=0
echo '=========== Start of Test script =================='
export MODEL_DIR=$PWD
echo "MODEL_DIR=$MODEL_DIR"

cd ../../
export FLAME_XPARSER_DIR=$PWD
echo "FLAME_XPARSER_DIR=$FLAME_XPARSER_DIR"

cd $FLAME_XPARSER_DIR
echo "Now in: $PWD"
echo '=========== START of XParser output =================='

#Parse in normal mode: DEBUG and SERIAL
if (($PARSE_DEBUG && $PARSE_SERIAL)); then
	./xparser -s $MODEL_DIR/model.xml
	echo '=========== END of XParser output =================='
	if (($BUILD)); then
		cd $MODEL_DIR
		make clean all
		echo 'Compilation done.'
		mv main main_sd
		echo 'Moved main ->main_sd'
	fi
fi

#Parse in parallel mode: DEBUG and PARALLEL
# NOTE: You need mpicc
if (($PARSE_DEBUG && $PARSE_PARALLEL)); then
	./xparser -p $MODEL_DIR/model.xml
	echo '=========== END of XParser output =================='
	if (($BUILD)); then
		cd $MODEL_DIR
		make clean all
		echo 'Compilation done.'
		mv main main_pd
		echo 'Moved main ->main_pd'
	fi
fi

#Parse in production mode: PRODUCTION and SERIAL
# NOTE: Compiler output does not produce as many warnings as debug mode
if (($PARSE_PRODUCTION && $PARSE_SERIAL)); then
	./xparser -s -f $MODEL_DIR/model.xml
	echo '=========== END of XParser output =================='
	if (($BUILD)); then
		cd $MODEL_DIR
		make clean all
		echo 'Compilation done.'
		mv main main_sf
		echo 'Moved main ->main_sf'
	fi
fi

#Parse in production mode: PRODUCTION and PARALLEL
# NOTE: You need mpicc
# NOTE: Compiler output does not produce as many warnings as debug mode
if (($PARSE_PRODUCTION && $PARSE_PARALLEL)); then
	./xparser -p -f $MODEL_DIR/model.xml
	echo '=========== END of XParser output =================='
	if (($BUILD)); then
		cd $MODEL_DIR
		make clean all
		echo 'Compilation done.'
		mv main main_pf
		echo 'Moved main ->main_pf'
	fi
fi

echo 'Test script done.'
