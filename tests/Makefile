# Makefile for run test xparser

CC         = gcc
CFLAGS     = -c -g -std=c99 -Wall -pedantic
LDFLAGS    = -lCUnit
SOURCES    = run_test.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = run_test
DEPS       = Makefile
RM         = rm -f

LIBMBOARD  = ../../../tags/libmboard/0.1.2-alpha

all: build run

run: test1/libmboard build
	./run_test

test1/libmboard:
	cd $(LIBMBOARD); ./autogen.sh; ./configure --prefix=`pwd`/../../../trunk/xparser/tests/test1/libmboard; make; make install; cd ../../../trunk/xparser/tests

build: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o run_test.exe run_test

vclean: clean
	$(RM) stderr.out stdout.out
	$(RM) -R test1/libmboard
	$(RM) test1/1.xml test1/log.xml
	if test -e test1/Makefile; \
		then make --directory=test1/ vclean; \
	fi 

