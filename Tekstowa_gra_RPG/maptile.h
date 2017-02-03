
#ifndef MAPTILE_INCLUDED
#define MAPTILE_INCLUDED

#include "item.h"
#include "enemy.h"
#include "player.h"

#include <iostream>
#include <vector>

struct Empty : public MapSquare{
	Empty(int a, int b)
		: MapSquare(a,b)
	{hole = false;}
};

struct StartingRoom: public MapSquare{
	StartingRoom(int a, int b)
	: MapSquare(a,b)
	{}

	virtual void intro_text() {
		std::cout << "You are in an awuful cave. Find exit!\n";
	}
	virtual void modify_player( Player&  p ) {}
};

struct ExitRoom: public MapSquare{
	ExitRoom(int a, int b)
		: MapSquare(a,b)
	{}
	virtual void intro_text() {
		std::cout << "Finally, you see a suhsine. Congratulations. You win\n";
	}
	virtual void modify_player( Player&  p ) {p.victory = true;}
};

struct LootRoom: public MapSquare{
	Item it;
	LootRoom(int a, int b, Item i)
	: MapSquare(a,b), it{i}
	{}

	void add_loot(Player & p){
		p.inventory.push_back(it);
	}
	void modify_player(Player & p){
		this->add_loot(p);
	}
	virtual void intro_text(){};
};

struct EnemyRoom: public MapSquare{


	EnemyRoom(int a, int b, Enemy e)
	: MapSquare(a,b,e)
	{}

	virtual void modify_player(Player & p){
		if( en.is_alive() ){
				p.hp -= en.damage;
				std::cout << en.name << " does " << en.damage <<" damage. You have "<< p.hp <<" HP remaining.\n";
		}
	}

	virtual void intro_text(){};
};

struct EmptyCave: public MapSquare{
	EmptyCave(int a, int b)
	: MapSquare(a,b)
	{}

	virtual void intro_text(){
		std::cout << "Empty place\n";
	}
	virtual void modify_player(Player & p){}
};

struct SkeletonRoom: public EnemyRoom{
	SkeletonRoom(int a, int b)
		:EnemyRoom(a,b,Skeleton())
	{}
	virtual void intro_text(){
		if(en.is_alive() )std::cout << "You see white man, no its a Skeleton\n";
		else std::cout << "Uff... You've already deal with that danger\n";
	}
};

struct GoblinRoom: public EnemyRoom{
	GoblinRoom(int a, int b)
		:EnemyRoom(a,b,Goblin())
	{}
	virtual void intro_text(){
		if(en.is_alive() )std::cout << "Small Goblin atacks you\n";
		else std::cout << "Uff... You've already deal with that danger\n";
	}
};

struct FindSwordRoom: public MapSquare{
	FindSwordRoom(int a, int b)
	: MapSquare(a,b)
	{}
	virtual void intro_text(){
		std::cout << "You see a golden chest, with a powerful hero weapon inside\n";
	}
	virtual void modify_player(Player & p){
		p.weap = Long_sword();
	}
};

struct Find5GoldRoom: public LootRoom{

	Find5GoldRoom(int a, int b)
	: LootRoom(a,b, Gold(5) )
	{}

	virtual void intro_text(){
		std::cout << "On the floor, you see some coins. Better pick it up\n";
	}
};



#endif
