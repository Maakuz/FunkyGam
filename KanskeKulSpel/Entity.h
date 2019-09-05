#pragma once
#include "SFML/Graphics.hpp"

class Entity : public sf::Sprite
{
public:
    Entity(sf::Vector2f pos, int health = 100, int speed = 10, int attack = 10);
    ~Entity() {};

protected:
    
    int health;
    int speed;

    int attack;
};