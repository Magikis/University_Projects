#include "world.h"
#include "maptile.h"
#include "player.h"
#include <iostream>

MapSquare* world[10][10]={
{new Empty(0,0), new Empty(0,1), new Empty(0,2), new Empty(0,3)},
{new Empty(1,0),new GoblinRoom(1,1), new ExitRoom(1,2), new SkeletonRoom(1,3), new Empty(1,4) },
{new Empty(2,0),new StartingRoom(2,1), new Find5GoldRoom(2,2), new FindSwordRoom(2,3), new Empty(2,4)},
{new Empty(3,0), new Empty(3,1), new Empty(3,2), new Empty(3,3)}
};
/*
void init_world() {
	  world.push_back(new Empty(0,0));          world.push_back(new Empty(0,1));        world.push_back(new Empty(0,2));        world.push_back(new SkeletonRoom(0,3)); world.push_back(new ExitRoom(0,4));
	  world.push_back(new Empty(1,0));          world.push_back(new Empty(1,1));        world.push_back(new Empty(1,2));        world.push_back(new FindSwordRoom(1,3));world.push_back(new SkeletonRoom(1,4));
	  world.push_back(new Find5GoldRoom(2,0));  world.push_back(new GoblinRoom(2,1));   world.push_back(new EmptyCave(2,2));    world.push_back(new Empty(2,3));        world.push_back(new Empty(2,4));
	  world.push_back(new Find5GoldRoom(3,0));  world.push_back(new EmptyCave(3,1));    world.push_back(new Empty(3,2));        world.push_back(new Empty(3,3));        world.push_back(new Empty(3,4));
	  world.push_back(new Empty(4,0));          world.push_back(new StartingRoom(4,1)); world.push_back(new Empty(4,2));        world.push_back(new Empty(4,3));        world.push_back(new Empty(4,4));
}
*/

int main()
{
    //init_world();
    std::cout << "What's your name character and start your journay\n";
    std::string temp;
    std::cin >> temp;
    Player player(temp,2,1);
    while(player.is_alive() && player.victory==false)
    {
        world[player.loc_x][player.loc_y]->intro_text();
        world[player.loc_x][player.loc_y]->modify_player(player);
        if (player.is_alive() && player.victory==false)
        {
            std::cout << "What do you do?\n";
            std::cin >> temp;
            if (temp == "N")
                player.move_n();
            else if (temp == "S")
                player.move_s();
            else if (temp == "E")
                player.move_e();
            else if (temp == "W")
                player.move_w();
            else if (temp == "Attack")
                player.attack( world[player.loc_x][player.loc_y]->en);
            else if (temp == "I")
                player.print_inventory();
            else if (temp == "Exit")
                return 0;
            else
                std::cout << "\n\nN -go north\nS -go south\nE -go east\nW -go west\nAttack -to attack a monster\nI -show inventory\nExit -turn of game\n\n";
        }
    }
        int score=0;
        for (auto i: player.inventory){ score+= i.value;}
        score += player.weap.value;
        std::cout << "Your score is: " << score << "gold coins \n";

    return 0;
}
