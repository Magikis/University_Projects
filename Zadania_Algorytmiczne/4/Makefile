CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow

all: CXXFLAGS += -O2
all: main
main: main.cpp

debug: CXXFLAGS += -g -DDEBUG
debug: main

clean:
	$(RM) main
