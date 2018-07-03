CC=cc
CFLAGS=-O2 -std=c11 -Wall

.PHONY: clean all test

all: librandutils.a

test: test_randutils

librandutils.a: randutils.o
	ar rcs librandutils.a randutils.o

randutils.o: randutils.c
	$(CC) $(CFLAGS) -c randutils.c

test_randutils: test_randutils.o
	$(CC) $(CFLAGS) -o test_randutils test_randutils.o -L. -lrandutils -lm

test_randutils.o: test_randutils.c librandutils.a randutils.h
	$(CC) $(CFLAGS) -c test_randutils.c

clean:
	rm -f randutils.o librandutils.a test_randutils.o test_randutils
