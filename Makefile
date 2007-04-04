# Makefile for xparser

CC=gcc
CFLAGS=-c -std=c99 -Wall -pedantic
LDFLAGS=
SOURCES=xparser.c memory.c readmodel.c dependencygraph.c parsetemplate.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=xparser
RM=rm -f

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o xparser.exe xparser

