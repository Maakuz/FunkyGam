#pragma once
#include "Entity.h"

class Player : public Entity 
{
public:
    Player(sf::Vector2f pos = sf::Vector2f(0, 0), int health = 100, int speed = 10, int attack = 10);
    ~Player() {};

    void update(float dt);

private:
    float acceleration;
    float friction;

    float lightBounceFreq;
    float lightBounceHeight;

};