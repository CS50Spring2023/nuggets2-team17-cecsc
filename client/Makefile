# Makefile for 'client' module
#
# Kyrylo Bakuemnko,	20 May 2023

.PHONY: all test valgrind clean

S = ../support
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$S
LIBS = $S/support.a

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all -s

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$S
CC = gcc
MAKE = make

all: client

client: client.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -lcurses -o $@

# indexer source dependencies
client.o:  $S/message.h

# test: client clienttest testing.sh testing.out
# 	bash -v testing.sh >& testing.out

# valgrind: client clienttest

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f client
	rm -f core
