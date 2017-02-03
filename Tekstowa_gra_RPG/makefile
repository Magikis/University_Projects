Flags = -Wpedantic -Wextra -Wall -std=c++11

main: main.o player.o
	g++ $(Flags) main.o player.o -o main 

main.o: main.cpp player.o world.h maptile.h player.h
	g++ $(Flags) -c main.cpp -o main.o
	
player.o: player.cpp world.h player.h
	g++ $(Flags) -c player.cpp -o player.o
clean:
	rm -f main *.o
