CFLAGS = -std=gnu99 -Wall -Wextra -Wshadow -fPIC
CC = gcc

all: tests fullTest

shared_debug: CFLAGS += -DDEBUG -g 
shared_debug: shared
shared:  CFLAGS += -D__REMOVE__FOO
shared: libmalloc.so 

debug: CFLAGS += -DDEBUG -g 
debug : all

validate.o: validate.c validate.h

arena.o: arena.h arena.c

malloc.o : malloc.c malloc.h

libmalloc.so: malloc.o arena.o
	$(CC) $(CFLAGS) -shared -o libmalloc.so arena.o malloc.o -pthread

tests: arena.o malloc.o

fullTest: validate.o arena.o malloc.o

clean:
	$(RM) *.o main tests *.so fullTest
