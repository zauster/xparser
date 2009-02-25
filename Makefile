# Makefile for FLAME xparser

CC         = gcc
CFLAGS     = -c -g -std=c99 -Wall -pedantic
LDFLAGS    =
SOURCES    = xparser.c memory.c readmodel.c dependencygraph.c parsetemplate.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = xparser
DEPS       = Makefile header.h
RM         = rm -f

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

test: all
	make --directory=tests/

testclean:
	$(RM) tests/*.o tests/run_test.exe tests/run_test tests/stderr.out tests/stdout.out

docs: docs/user_manual/user_manual.ps docs/developer_manual/developer_manual.ps

docs/user_manual/user_manual.ps:
	make --directory=docs/user_manual/

docs/developer_manual/developer_manual.ps:
	make --directory=docs/developer_manual/

docsclean:
	make --directory=docs/user_manual/ clean
	make --directory=docs/developer_manual/ clean

clean:
	$(RM) *.o xparser.exe xparser

vclean: testclean docsclean clean
