#include "player.h"
#include "world.h"

#include <iostream>

extern MapSquare* world[10][10];

void Player::move(int cx, int cy){
		/*if(WORLD_SIZE * (cy+loc_y) + cx+loc_x < 0 || WORLD_SIZE * (cy+loc_y) + cx+loc_x >= WORLD_SIZE * WORLD_SIZE){
			std::cout << "Not Allowed Move\n";
			return;
		}*/
		if( world[loc_x+cx][loc_y+cy]->hole == 0 )
		{
			std::cout << "Wall, try again\n";
			return;
		}
		loc_x += cx;
		loc_y += cy;
}

void Player::attack(Enemy & enem){
	std::cout << "You use " << weap.name << " aginst " << enem.name << " !\n";
	enem.hp -= weap.damage;
	if(enem.is_alive())	std::cout << enem.name << " HP is " << enem.hp << "\n";
	else std::cout << "You killed " << enem.name << " \n";
}

int Player::place()
{
    return loc_y*WORLD_SIZE + loc_x;
}

