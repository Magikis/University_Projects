#Kacper Kulczak 279079
CXXFLAGS = -std=gnu++11 -Wall -Wextra -Wshadow
CXX = g++

all: main

main: main.cpp  window.o part.o #comunication.o

#comunication.o : comunication.cpp comunication.h

window.o: window.cpp window.h

part.o: part.cpp part.h

debug: CXXFLAGS += -DDEBUG -g
debug : all

clean:
	$(RM) *.o
distclean: clean
	$(RM) main
