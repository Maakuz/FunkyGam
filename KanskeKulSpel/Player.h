#pragma once
#include "AnimatedEntity.h"

class Player : public AnimatedEntity 
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