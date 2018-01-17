Tests for FLAME function conditions
Sander van der Hoog (svdhoog@gmail.com)
16 Jan 2018

This is a suite of unittests to test function conditions for FLAME models.

The folder xparser/tests/test_conditions contains 16 test models.
See the readme.rd file in that folder for how to reproduce the tests.
Each model folder contains a distinct model.xml and 0.xml file.
The agent functions are the same for all tests, and only have one agent with two functions that either print out TRUE or FALSE.
Use the xparser in this repository's root (make sure to build the xparser executable first).

Folders:

- doc/tests_conditions.tex documents a description of all 64 tests.

Run pdflatex tests_conditions.tex to generate tests_conditions.pdf

- models/test_n contains model.xml and 0.xml files.

- parse.sh parses and compiles all tests.

- run_all_tests.sh runs the testing suite.

- output/test_output.txt contains the output of the testing suite.

Output should be compared to the expected output in Table 1 in doc/tests_conditions.pdf


