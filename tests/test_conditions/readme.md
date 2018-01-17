Tests for FLAME function conditions
Sander van der Hoog (svdhoog@gmail.com)
16 Jan 2018

This is a suite of unittests to test function conditions for FLAME models.


Folders:

- doc/tests_conditions.pdf for a description of the unittests.

- tests/test_n contain model.xml and 0.xml files.

- run_tests contains a testing suite to run all tests.

- output contains stdout.txt of the testing suite


The folder xparser/tests/test_conditions contains 16 test models.
See the readme.rd file in that folder for how to reproduce the tests.
Each model folder contains a distinct model.xml and 0.xml file.
The agent functions are the same for all tests, and only have one agent with two functions that either print out TRUE or FALSE.
Use the xparser in this repository's root (make sure to build the xparser executable first).
