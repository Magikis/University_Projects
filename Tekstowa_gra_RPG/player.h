#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

#include "item.h"
#include "enemy.h"

struct Player
{
    std::string name;
    std::vector<Item> inventory;
    Weapon weap;
    int hp;
    int loc_x, loc_y;
    bool victory;

    Player(std::string a,int start_x, int start_y)
        : name {a},hp {20}, loc_x {start_x}, loc_y {start_y}, victory {0}, weap {Short_sword()}
    {
        inventory.push_back(Gold(15));
    }

    bool is_alive()
    {
        return hp > 0;
    }

    void print_inventory()
    {
        std::cout << "Your's inventory:\n";
        for( auto i : inventory )
        {
            std::cout << i;
        }
        std::cout << weap;
    }
    void move(int cx, int cy);
    void move_n()
    {
        this->move(-1,0);
    }
    void move_s()
    {
        this->move(1,0);
    }
    void move_w()
    {
        this->move(0,-1);
    }
    void move_e()
    {
        this->move(0,1);
    }

    void attack(Enemy & enem);
    int place();
};


#endif

