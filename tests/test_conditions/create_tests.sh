#!/bin/bash

#export TESTS=$(seq 2 1 16)
export TESTS=$(seq 1 1 16)

cd models

# Copy files from test_1
for i in $TESTS; do

	mkdir -p test_$i/a
	cp test_$i/ab/model_ab.xml test_$i/a/model.xml
	cp templates/a.xml test_$i/a/0.xml
	cp templates/agent_functions.c test_$i/a/agent_functions.c

	mkdir -p test_$i/b
	cp test_$i/ab/model_ab.xml test_$i/b/model.xml
	cp templates/b.xml test_$i/b/0.xml
	cp templates/agent_functions.c test_$i/b/agent_functions.c
	
	mkdir -p test_$i/c
	cp test_$i/cd/model_cd.xml test_$i/c/model.xml
	cp templates/c.xml test_$i/c/0.xml
	cp templates/agent_functions.c test_$i/c/agent_functions.c
	
	mkdir -p test_$i/d
	cp test_$i/cd/model_cd.xml test_$i/d/model.xml
	cp templates/d.xml test_$i/d/0.xml
	cp templates/agent_functions.c test_$i/d/agent_functions.c
	
done

cd -