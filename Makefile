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

test:
	make --directory=tests/

docs: docs/user_manual/user_manual.ps docs/developer_manual/developer_manual.ps

docs/user_manual/user_manual.ps:
	make --directory=docs/user_manual/

docs/developer_manual/developer_manual.ps:
	make --directory=docs/developer_manual/

clean:
	$(RM) *.o xparser.exe xparser

docsclean:
	make --directory=docs/user_manual/ clean
	make --directory=docs/developer_manual/ clean

