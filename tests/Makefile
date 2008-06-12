# Makefile for run test xparser

CC         = gcc
CFLAGS     = -c -g -std=c99 -Wall -pedantic
LDFLAGS    = -lCUnit
SOURCES    = run_test.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = run_test
DEPS       = Makefile
RM         = rm -f

all: build run

run: build
	./run_test

build: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o run_test.exe run_test

