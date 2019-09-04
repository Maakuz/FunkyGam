#pragma once
#include "Entity.h"

class Player : public Entity 
{
public:
    Player(sf::Texture* texture, sf::Vector2i pos, int health = 100, int speed = 10, int attack = 10);
    Player(sf::Vector2i pos = sf::Vector2i(0, 0), int health = 100, int speed = 10, int attack = 10);
    ~Player() {};

private:
};