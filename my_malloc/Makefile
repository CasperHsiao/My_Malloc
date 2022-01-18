CC=gcc
# CFLAGS=-O3 -fPIC
CFLAGS=-ggdb3
DEPS=my_malloc.h

all: lib test

test: test_my_malloc.o my_malloc.o
	$(CC) $(CFLAGS) -o test test_my_malloc.o my_malloc.o

lib: my_malloc.o
	$(CC) $(CFLAGS) -shared -o libmymalloc.so my_malloc.o

%.o: %.c my_malloc.h
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f *~ *.o *.so test

clobber:
	rm -f *~ *.o
