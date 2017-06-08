#kacper kulczak 279079
CC = gcc
CFLAGS = -Wall -W -std=c99 -Wshadow -Wextra -g

router: utils.o sending.o receiving.o

utils.o: utils.c utils.h

sending.o: sending.c sending.h utils.o

receiving.o: receiving.c receiving.h utils.o

clean:
	$(RM) *.o *.gch
distclean:
	$(RM) router *.o
