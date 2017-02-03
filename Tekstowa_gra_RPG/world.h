#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED

#include "enemy.h"
#include <vector>
#include <iostream>

#define WORLD_SIZE 5

struct Player;

struct MapSquare{
	int x,y;
	bool hole;
	Enemy en;
	MapSquare(int a, int b)
		: x{a}, y{b}, en{NoEnemy()}, hole{true}
	{}
	MapSquare(int a, int b, Enemy e)
		: x{a}, y{b}, en{e}, hole{true}
	{}

	virtual void intro_text(){std::cout << "BUG\n";}

	virtual void modify_player(Player &p) {std::cout << "BUG\n";}
};



#endif

