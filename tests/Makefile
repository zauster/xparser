# Makefile for run test xparser

CC         = gcc
CFLAGS     = -c -g -std=c99 -Wall -pedantic -posix
LDFLAGS    = -lcunit
SOURCES    = run_test.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = run_test
DEPS       = Makefile
RM         = rm -f

LIBMBOARD  = /Users/stc/workspace/libmboard/

all: build run

#run: test1/libmboard build
run: build
	./run_test

test1/libmboard:
	cd $(LIBMBOARD); ./autogen.sh; ./configure --prefix=`pwd`/../../../trunk/xparser/tests/test1/libmboard; make; make install; cd ../../../trunk/xparser/tests

build: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o run_test.exe run_test

