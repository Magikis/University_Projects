#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED

#include <string>

struct Enemy{
	std::string name;
	int hp, damage;
	Enemy(std::string a, int b, int c)
		: name{a}, hp{b}, damage{c}
	{}

	bool is_alive(){
		return hp > 0;
	}
};

struct Goblin: public Enemy{
	Goblin()
		:Enemy("Goblin", 10, 1)
	{}
};

struct Skeleton: public Enemy{
	Skeleton()
		:Enemy("Skeleton", 20, 4)
	{}
};

struct NoEnemy: public Enemy{
	NoEnemy()
		:Enemy("",0,0)
	{}
};

#endif

