#ifndef ITEM_INCLUDED
#define ITEM_INCLUDED

#include <string>
#include <iostream>

struct Item
{
    std::string name;
    std::string description;
    int value;

    Item(std::string a, std::string b, int c)
        :	name {a},
      description {b},
      value {c}
    {}

};

struct Gold : public Item
{
    Gold(int a)
        : Item("Gold","A gold coin", a)
    {}
};

struct Weapon : public Item
{
    int damage;
    Weapon(std::string a, std::string b, int c, int d)
        :	Item(a,b,c), damage {d}
    {}
};

struct Short_sword : public Weapon
{
    Short_sword()
        : Weapon("Short Sword", "Short green dirty sword", 1, 5)
    {}
};

struct Long_sword : public Weapon
{
    Long_sword()
        :Weapon("Long Sword", "Heavy, beautiful steel long sword", 50, 10)
    {}
};

inline
std::ostream& operator << (std::ostream & stream, const Item& i)
{
    stream << "It's a " << i.name << ".\t" << i.description << "\n Value: "<< i.value << "\n";
    return stream;
}

inline
std::ostream& operator << (std::ostream & stream, const Weapon& i)
{
    stream << "It's a " << i.name << ".\t" << i.description << "\n Value: "<< i.value << "\t" << "Damage: " << i.damage <<"\n";
    return stream;
}

#endif
